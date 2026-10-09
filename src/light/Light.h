#pragma once

#include <Arduino.h>
#include <FastLED.h>

class Light {
public:
  void Initialize();
  void Update(ulong deltaTime);
  void SetLeftGunLight(CRGB color);
  void SetRightGunLight(CRGB color);
  void SetEyeColor(CRGB color);

private:
  CRGB centerLeds[9];
  CRGB leftLeds[2];
  CRGB rightLeds[2];
};