#include "Audio.h"

#define I2S_PORT I2S_NUM_0
#define SAMPLE_RATE 44100

Audio::Audio() : source("/", ".mp3"), player(source, i2s, decoder) {}

void Audio::Initialize() {

  source.selectStream("/fire.mp3");

  auto cfg = i2s.defaultConfig(TX_MODE);
  cfg.pin_bck = PIN_BCLK;
  cfg.pin_ws = PIN_LRCLK;
  cfg.pin_data = PIN_DIN;
  cfg.pin_mck = -1;
  cfg.sample_rate = 44100;
  cfg.bits_per_sample = 16;
  cfg.channels = 2;
  cfg.buffer_count = 8;
  cfg.buffer_size = 512;
  cfg.use_apll = false;
  cfg.auto_clear = true;
  cfg.fixed_mclk = 0;
  i2s.begin(cfg);
  player.begin();
}

void Audio::Update(ulong deltaTime) {

  player.copy();
}