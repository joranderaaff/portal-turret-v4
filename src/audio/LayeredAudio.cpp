#include "LayeredAudio.h"

LayeredAudio::LayeredAudio(const uint8_t *samplesIn, int sampleCountIn) {
  samples = samplesIn;
  sampleCount = sampleCountIn;
}

void LayeredAudio::Read(uint8_t *buffer, int len) {
  len &= ~1; // whole 16-bit samples only

  for (int w = 0; w < len; w += 2) {

    int32_t sample = 0;

    for (int instance = ringStart; instance <= ringEnd; instance++) {
      // Read (little-endian int16) and add to the current sample
      int readIndex = readIndexRingBuffer[instance % RING_COUNT];

      sample += (int16_t)(samples[readIndex * 2] | samples[readIndex * 2 + 1] << 8);

      readIndexRingBuffer[instance % RING_COUNT] = readIndex + 1;
    }

    // Shots all have the same length, so they finish in the order they started
    while (ringStart <= ringEnd && readIndexRingBuffer[ringStart % RING_COUNT] >= sampleCount) {
      ringStart++;
    }

    sample = constrain(sample, INT16_MIN, INT16_MAX);
    buffer[w] = sample & 0xFF;
    buffer[w + 1] = (sample >> 8) & 0xFF;
  }
}

void LayeredAudio::Trigger() {
  if (ringEnd - ringStart + 1 >= RING_COUNT) {
    return; // ring is full, drop this shot
  }
  ringEnd++;
  readIndexRingBuffer[ringEnd % RING_COUNT] = 0;
}

bool LayeredAudio::IsPlaying() {
  return ringEnd >= ringStart;
}
