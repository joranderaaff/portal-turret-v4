#pragma once

#include <Arduino.h>

inline float Lerp(float a, float b, float t) {
  return a + t * (b - a);
}

inline float Clamp(float t, float min, float max) {
  return t < min ? min : (t > max ? max : t);
}

inline float Damp(float current, float target, float lambda, float dt) {
  return Lerp(target, current, exp(-lambda * dt));
}

inline int Sign(float a) {
  return a >= 0.0f ? 1 : -1;
}