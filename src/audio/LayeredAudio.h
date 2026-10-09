#pragma once

#include <Arduino.h>

#define RING_COUNT 64

class LayeredAudio {
public:
  LayeredAudio(const uint8_t *samplesIn, int sampleCount);
  void Read(uint8_t *buffer, int len);
  void Trigger();
  bool IsPlaying();

private:
  const uint8_t *samples;
  int sampleCount;
  int readIndexRingBuffer[RING_COUNT];
  int ringStart = 0;
  int ringEnd = -1;
};