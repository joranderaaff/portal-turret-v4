#include "Audio.h"

#define SAMPLE_RATE 22050
#define QUEUE_SIZE 5

Audio::Audio() : source("/", ".mp3"), decoder(&volumeStream, &mp3Decoder), ShootAudio(samples, 773, 5065, sizeof(samples) / 2) {}

void Audio::Initialize() {

  audioCommandQueue = xQueueCreate(QUEUE_SIZE, sizeof(uint16_t));
  loopedAudioCommandQueue = xQueueCreate(QUEUE_SIZE, sizeof(uint16_t));

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

  auto vcfg = volumeStream.defaultConfig();
  vcfg.copyFrom(cfg);
  volumeStream.begin(vcfg);
  volumeStream.setVolume(0.5f);

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
  return isPlaying || (currentLoopedAudio != nullptr && currentLoopedAudio->IsPlaying());
}

AudioLoop *Audio::GetLoopendSoundByType(AudioLoopId type) {
  return &ShootAudio;
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

  if (isPlaying) {
    if (!copier.copy()) {
      decoder.end();
      isPlaying = false;
    }
  } else if (currentLoopedAudio && currentLoopedAudio->IsPlaying()) {
    int bytesAvailableForWrite = min(i2s.availableForWrite(), (int)sizeof(sampleBuffer));
    currentLoopedAudio->Read(sampleBuffer, bytesAvailableForWrite);
    volumeStream.write(sampleBuffer, bytesAvailableForWrite);
  }
}

void Audio::AudioTask(void *arg) {
  auto *self = static_cast<Audio *>(arg);
  for (;;) {
    self->Update(0);
    vTaskDelay(1);
  }
}