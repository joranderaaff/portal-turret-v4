#pragma once

#include <Arduino.h>
#include <FastLED.h>

class Light {
public:
  void Initialize();
  void Update(ulong deltaTime);

private:
  CRGB centerLeds[9];
  CRGB leftLeds[2];
  CRGB rightLeds[2];
};

extern Light lights;