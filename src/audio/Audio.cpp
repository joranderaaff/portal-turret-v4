#include "Audio.h"

#define SAMPLE_RATE 22050

Audio::Audio() : source("/", ".mp3"), decoder(&i2s, &mp3Decoder), ShootAudio(samples, 773, 5065, sizeof(samples) / 2) {}

void Audio::Initialize() {
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

  source.begin();
  decoder.begin();

  copier.resize(256);

  xTaskCreatePinnedToCore(AudioTask, "audio", 8192, this, 2, nullptr, 0);
}

void Audio::RequestGunSoundChange(GunAudioRequestType requestType) {
  playGunAudio = requestType;
}

void Audio::RequestSound(AudioType type) {
  requestedAudio = type;
  isPlaying = true;
}

void Audio::GetRandomAudio(AudioType type) {
  long filenum;
  switch (type) {
  case AudioType::Activate:
    filenum = random(8) + 1;
    snprintf(filename, 31, "/01_activate/%03i.mp3", filenum);
    break;
  case AudioType::Searching:
    filenum = random(10) + 1;
    snprintf(filename, 31, "/07_search/%03i.mp3", filenum);
    break;
  case AudioType::Pickup:
    filenum = random(10) + 1;
    snprintf(filename, 31, "/05_pickup/%03i.mp3", filenum);
    break;
  case AudioType::Tipped:
    filenum = random(6) + 1;
    snprintf(filename, 31, "/08_tipped/%03i.mp3", filenum);
    break;
  case AudioType::Retire:
    filenum = random(7) + 1;
    snprintf(filename, 31, "/06_retire/%03i.mp3", filenum);
    break;
  default:
    strcpy(filename, "/09/001.mp3");
    break;
  }
}

bool Audio::IsPlaying() {
  return requestedAudio != AudioType::None || isPlaying;
}

void Audio::AudioTask(void *arg) { // declare as static in Audio.h
  auto *self = static_cast<Audio *>(arg);
  for (;;) {
    self->Update(0);
    vTaskDelay(1); // yield so core 0 isn't starved (WiFi lives there)
  }
}

void Audio::Update(ulong deltaTime) {

  GunAudioRequestType gunAudioRequest = playGunAudio.exchange(GunAudioRequestType::None);
  if (gunAudioRequest == GunAudioRequestType::Start) {
    ShootAudio.Begin();
  }
  if (gunAudioRequest == GunAudioRequestType::Stop) {
    ShootAudio.Stop();
  }

  AudioType nextAudioType = requestedAudio.exchange(AudioType::None);
  if (nextAudioType != AudioType::None) {
    decoder.begin();
    GetRandomAudio(nextAudioType);
    Stream *file = source.selectStream(filename);
    copier.begin(decoder, *file);
    isPlaying = true;
  }

  if (ShootAudio.IsPlaying()) {
    int bytesAvailableForWrite = min(i2s.availableForWrite(), (int)sizeof(sampleBuffer));
    ShootAudio.Read(sampleBuffer, bytesAvailableForWrite);
    i2s.write(sampleBuffer, bytesAvailableForWrite);
  } else {
    if (isPlaying) {
      if (!copier.copy()) {
        isPlaying = false;
      }
    }
  }
}