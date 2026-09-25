#include <Arduino.h>

float Lerp(float a, float b, float t) {
  return a + t * (b - a);
}

float Damp(float current, float target, float lambda, float dt) {
  return Lerp(target, current, exp(-lambda * dt));
}