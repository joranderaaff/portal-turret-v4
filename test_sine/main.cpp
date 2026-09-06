// Bisect test for choppy audio.
//
// Feeds a generated 440 Hz sine straight into I2S. There is no LittleFS, no
// file read and no MP3 decoder in this path - only the I2S peripheral and its
// DMA.
//
//   clean sine  -> I2S/DMA are fine, the problem is the decode / file read
//   choppy sine -> the problem is the I2S config, DMA depth or clocking,
//                  and has nothing to do with MP3
//
// Build and flash with:  pio run -e sine_test -t upload
//
// Keep the cfg below identical to the one in src/audio/Audio.cpp, otherwise
// the test is not measuring the same thing you are running.

#include "AudioTools.h"
#include "pins.h"
#include <Arduino.h>

AudioInfo info(44100, 2, 16);

I2SStream i2s;
SineWaveGenerator<int16_t> sine(16000); // amplitude, ~half of full scale
GeneratedSoundStream<int16_t> sound(sine);
StreamCopy copier(i2s, sound);

void setup() {
  Serial.begin(115200);
  delay(2000); // let USB CDC enumerate before we print
  Serial.println("--- I2S sine test ---");

  auto cfg = i2s.defaultConfig(TX_MODE);
  cfg.copyFrom(info);

  cfg.pin_bck = PIN_BCLK;
  cfg.pin_ws = PIN_LRCLK;   // NOT pin_mck - the MAX98357A has no MCLK input
  cfg.pin_data = PIN_DIN;
  cfg.pin_mck = -1;

  // Library defaults are 6 x 512 = 3072 bytes = ~17 ms of headroom at
  // 44100/16/2. Bump these to widen the margin and see if the gaps go away.
  cfg.buffer_count = 6;
  cfg.buffer_size = 512;

  if (!i2s.begin(cfg)) {
    Serial.println("i2s.begin FAILED");
    return;
  }

  sine.begin(info, 440.0f);

  Serial.printf("running: %d Hz, %d ch, %d bit, dma %d x %d = %d bytes\n",
                (int)cfg.sample_rate, cfg.channels, cfg.bits_per_sample,
                cfg.buffer_count, cfg.buffer_size,
                cfg.buffer_count * cfg.buffer_size);
}

void loop() {
  ulong t = micros();
  size_t n = copier.copy();
  ulong dt = micros() - t;

  // copier.copy() blocks on the DMA, so in a healthy steady state the average
  // tracks the real time of the audio produced. A low average means we are
  // returning without having filled the DMA - i.e. an underrun.
  static ulong acc = 0;
  static ulong worst = 0;
  static int cnt = 0;

  acc += dt;
  if (dt > worst)
    worst = dt;

  if (++cnt >= 200) {
    Serial.printf("copy avg %lu us, worst %lu us, last %u bytes\n", acc / cnt,
                  worst, (unsigned)n);
    acc = 0;
    worst = 0;
    cnt = 0;
  }
}
