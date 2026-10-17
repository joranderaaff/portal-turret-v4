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
  // Forces every LED to this color, overriding all other callers (diagnose page).
  void SetOverrideColor(CRGB color);

private:
  bool overrideActive = false;
  CRGB overrideColor;
  CRGB centerLeds[9];
  CRGB leftLeds[2];
  CRGB rightLeds[2];
};