#include "Audio.h"

#define SAMPLE_RATE 22050
#define QUEUE_SIZE 5

namespace {
// Shots are mixed into the I2S DMA queue until about this many bytes are waiting (1024 = ~23 ms)
constexpr int SHOT_QUEUE_TARGET_BYTES = 1024;
// Don't bother mixing smaller top-ups than this (256 = ~6 ms)
constexpr int SHOT_MIN_WRITE_BYTES = 256;
constexpr int BLOCKING_WRITE_TIMEOUT_MS = 100;
// 16 bit mono: bytes the I2S peripheral consumes per microsecond
constexpr float BYTES_PER_US = SAMPLE_RATE * 2 / 1000000.0f;

const int loopSections[2]{
    774,
    5063,
};
}

Audio::Audio(Settings &_settings) : settings(_settings), source("/", ".mp3"), decoder(&volumeStream, &mp3Decoder), ShootAudio(samples, sizeof(samples) / 2) {}

void Audio::Initialize() {

  audioCommandQueue = xQueueCreate(QUEUE_SIZE, sizeof(AudioId));
  loopedAudioCommandQueue = xQueueCreate(QUEUE_SIZE, sizeof(AudioLoopId));
  layeredAudioCommandQueue = xQueueCreate(QUEUE_SIZE, sizeof(LayeredAudioId));

  auto cfg = i2s.defaultConfig(TX_MODE);
  cfg.pin_bck = PIN_BCLK;
  cfg.pin_ws = PIN_LRCLK;
  cfg.pin_data = PIN_DIN;
  cfg.pin_mck = -1;
  cfg.sample_rate = SAMPLE_RATE;
  cfg.bits_per_sample = 16;
  cfg.channels = 1;
  cfg.buffer_count = 8;
  cfg.buffer_size = 512;
  cfg.use_apll = false;
  cfg.auto_clear = true;
  cfg.fixed_mclk = 0;
  i2s.begin(cfg);
  SetWriteBlocking(true);

  auto vcfg = volumeStream.defaultConfig();
  vcfg.copyFrom(cfg);
  volumeStream.begin(vcfg);
  volumeStream.setVolume(settings.GetFloat(SettingId::AudioVolume));

  source.begin();
  decoder.begin();

  copier.resize(256);

  xTaskCreatePinnedToCore(AudioTask, "audio", 8192, this, 2, nullptr, 0);
}

void Audio::QueueAudioCommand(AudioId nextAudio) {
  xQueueSend(audioCommandQueue, &nextAudio, 0);
}

void Audio::QueueLoopedAudioCommand(AudioLoopId nextAudio) {
  xQueueSend(loopedAudioCommandQueue, &nextAudio, 0);
}

void Audio::QueueLayeredAudioCommand(LayeredAudioId nextAudio) {
  xQueueSend(layeredAudioCommandQueue, &nextAudio, 0);
}

void Audio::GetRandomSoundByType(AudioId type) {
  long filenum;
  switch (type) {
  case AudioId::Activate:
    filenum = random(8) + 1;
    snprintf(filename, 31, "/01_activate/%03i.mp3", filenum);
    break;
  case AudioId::Searching:
    filenum = random(10) + 1;
    snprintf(filename, 31, "/07_search/%03i.mp3", filenum);
    break;
  case AudioId::Pickup:
    filenum = random(10) + 1;
    snprintf(filename, 31, "/05_pickup/%03i.mp3", filenum);
    break;
  case AudioId::Tipped:
    filenum = random(6) + 1;
    snprintf(filename, 31, "/08_tipped/%03i.mp3", filenum);
    break;
  case AudioId::Retire:
    filenum = random(7) + 1;
    snprintf(filename, 31, "/06_retire/%03i.mp3", filenum);
    break;
  case AudioId::Alarm:
    strcpy(filename, "/09/001.mp3");
    break;
  default:
    strcpy(filename, "/09/001.mp3");
    break;
  }
}

bool Audio::IsPlaying() {
  return isPlaying || ShootAudio.IsPlaying() || (currentLoopedAudio != nullptr && currentLoopedAudio->IsPlaying());
}

AudioLoop *Audio::GetLoopendSoundByType(AudioLoopId type) {
  return nullptr;
}

void Audio::Update(ulong deltaTime) {

  AudioLoopId nextLoopedAudioType = AudioLoopId::None;
  if (xQueueReceive(loopedAudioCommandQueue, &nextLoopedAudioType, 0)) {
    if (currentLoopedAudio && nextLoopedAudioType == AudioLoopId::None) {
      currentLoopedAudio->Stop();
    }

    if (nextLoopedAudioType != AudioLoopId::None) {
      currentLoopedAudio = GetLoopendSoundByType(nextLoopedAudioType);
      currentLoopedAudio->Begin();
    }
  }

  LayeredAudioId nextLayeredAudioType;
  if (xQueueReceive(layeredAudioCommandQueue, &nextLayeredAudioType, 0)) {
    if (nextLayeredAudioType == LayeredAudioId::Trigger) {
      ShootAudio.Trigger();
    }
  }

  AudioId nextAudioType = AudioId::None;
  if (xQueueReceive(audioCommandQueue, &nextAudioType, 0)) {
    if (nextAudioType != AudioId::None) {
      decoder.begin();
      GetRandomSoundByType(nextAudioType);
      Stream *file = source.selectStream(filename);
      copier.begin(decoder, *file);
      isPlaying = true;
    }
  }

  if (ShootAudio.IsPlaying() || pendingBytes > 0) {
    // Non-blocking: only top up the DMA queue to a shallow fill level. Whatever is queued
    // plays before a new shot can be heard, so a shallow queue keeps trigger latency low.
    SetWriteBlocking(false);

    // i2s.availableForWrite() can't be used for the fill level: the driver counts the silence it
    // auto-clears as "sent" and never resyncs, so after idle time it reports the queue as empty.
    // Estimate the fill from what was accepted and how much has played since (22050 Hz, 16 bit).
    ulong now = micros();
    queuedBytes = max(0.0f, queuedBytes - (now - lastQueueUpdateUs) * BYTES_PER_US);
    lastQueueUpdateUs = now;

    if (pendingBytes == 0) {
      int len = min((int)(SHOT_QUEUE_TARGET_BYTES - queuedBytes), (int)sizeof(sampleBuffer)) & ~1;
      if (len >= SHOT_MIN_WRITE_BYTES) {
        ShootAudio.Read(sampleBuffer, len);
        // First attempt goes through the volume stream, which scales the buffer in place
        size_t written = volumeStream.write(sampleBuffer, len);
        queuedBytes += written;
        pendingBytes = len - written;
        if (pendingBytes > 0) {
          memmove(sampleBuffer, sampleBuffer + written, pendingBytes);
        }
      }
    } else {
      // The queue didn't take everything: retry the rest (already scaled, so skip the volume stream)
      size_t written = i2s.write(sampleBuffer, pendingBytes);
      queuedBytes += written;
      pendingBytes -= written;
      if (pendingBytes > 0) {
        memmove(sampleBuffer, sampleBuffer + written, pendingBytes);
      }
    }
  } else if (isPlaying) {
    // The decoder writes chunks of unknown size and can't handle a partial write
    SetWriteBlocking(true);
    if (!copier.copy()) {
      decoder.end();
      isPlaying = false;
    }
  } else if (currentLoopedAudio && currentLoopedAudio->IsPlaying()) {
    SetWriteBlocking(true);
    int len = min(i2s.availableForWrite(), (int)sizeof(sampleBuffer)) & ~1;
    currentLoopedAudio->Read(sampleBuffer, len);
    volumeStream.write(sampleBuffer, len);
  }
}

void Audio::SetWriteBlocking(bool blocking) {
  if (blocking == writeBlocking) {
    return;
  }
  writeBlocking = blocking;
  static_cast<I2SDriver *>(i2s.driver())->setWaitTimeWriteMs(blocking ? BLOCKING_WRITE_TIMEOUT_MS : 0);
}

void Audio::AudioTask(void *arg) {
  auto *self = static_cast<Audio *>(arg);
  for (;;) {
    self->Update(0);
    vTaskDelay(1);
  }
}