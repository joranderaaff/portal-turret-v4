#include "Audio.h"

#define I2S_PORT I2S_NUM_0
#define SAMPLE_RATE 44100

void Audio::Initialize() {
  i2s_config_t i2s_config = {.mode =
                                 (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
                             .sample_rate = SAMPLE_RATE,
                             .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
                             .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
                             .communication_format = I2S_COMM_FORMAT_STAND_I2S,
                             .intr_alloc_flags = 0,
                             .dma_buf_count = 8,
                             .dma_buf_len = 64,
                             .use_apll = false,
                             .tx_desc_auto_clear = true,
                             .fixed_mclk = 0};

  i2s_pin_config_t pin_config = {.bck_io_num = PIN_BCLK,
                                 .ws_io_num = PIN_LRCLK,
                                 .data_out_num = PIN_DIN,
                                 .data_in_num = I2S_PIN_NO_CHANGE};

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_PORT, &pin_config);
}

void Audio::Update(ulong deltaTime) {
  int toneFreq = 880;
  static float phase = 0;
  const int samples = 256;

  int16_t buffer[samples * 2]; // stereo

  for (int i = 0; i < samples; i++) {
    int16_t sample = (int16_t)(sin(phase) * 1200);
    // int16_t sample = (int16_t)(sin(phase) * 12000);

    // Left and right channels
    buffer[i * 2] = sample;
    buffer[i * 2 + 1] = sample;

    phase += 2.0 * PI * toneFreq / SAMPLE_RATE;

    if (phase >= 2.0 * PI) {
      phase -= 2.0 * PI;
    }
  }
  size_t bytes_written;
  i2s_write(I2S_PORT, buffer, sizeof(buffer), &bytes_written, portMAX_DELAY); //
}