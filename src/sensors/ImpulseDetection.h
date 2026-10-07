#pragma once

#include "helpers/Math.h"
#include <Arduino.h>

class ImpulseDetection {
public:
  ImpulseDetection();
  // Adds a sample and returns true when the latest window matches the impulse.
  bool AddSample(float z);
  float GetCurrentAmplitudeZ();

private:
  // Number of values in the impulse template; the constructor list must match.
  static const int impulseLength = 32;
  // Shortest and longest window that is tried, in samples.
  static const int minWindowSize = 20;
  static const int maxWindowSize = 30;
  // History kept, must hold the longest window.
  static const int sampleCount = maxWindowSize;

  float currentAmplitudeZ = 0;

  int currentIndex = 0;
  float samples[sampleCount];
  float impulse[impulseLength];
};
