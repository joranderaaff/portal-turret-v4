#include "Audio.h"

#define I2S_PORT I2S_NUM_0
#define SAMPLE_RATE 44100

Audio::Audio()
    : source("/", ".mp3"), decoder(&i2s, &mp3Decoder),
      ShootAudio(samples, 1537, 10102) {}

void Audio::Initialize() {
  auto cfg = i2s.defaultConfig(TX_MODE);
  cfg.pin_bck = PIN_BCLK;
  cfg.pin_ws = PIN_LRCLK;
  cfg.pin_data = PIN_DIN;
  cfg.pin_mck = -1;
  cfg.sample_rate = 44100;
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
}

void Audio::PlaySound(AudioType type) {
  GetRandomAudio(type);
  Stream *file = source.selectStream(filename);
  copier.begin(decoder, *file);
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
  return isPlaying;
}

void Audio::Update(ulong deltaTime) {
  if (ShootAudio.IsPlaying()) {
    int bytesAvailableForWrite = i2s.availableForWrite();
    ShootAudio.Read(sampleBuffer, bytesAvailableForWrite);
    i2s.write(sampleBuffer, bytesAvailableForWrite);
  } 
  else {
    if(isPlaying && !copier.copy()) {
      isPlaying = false;
    }
  }
}