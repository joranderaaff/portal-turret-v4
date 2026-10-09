#include "Light.h"
#include "pins.h"

void Light::Initialize() {
  FastLED.addLeds<WS2812, PIN_NEOPIXEL_CENTER, GRB>(centerLeds, 9);
  FastLED.addLeds<WS2812, PIN_NEOPIXEL_LEFT, RGB>(leftLeds, 2);
  FastLED.addLeds<WS2812, PIN_NEOPIXEL_RIGHT, RGB>(rightLeds, 2);

  for (int i = 0; i < 8; ++i) {
    centerLeds[i] = CRGB(32, 0, 0);
  }
  centerLeds[8] = CRGB::Red;

  std::swap(centerLeds[8].r, centerLeds[8].g);
  FastLED.show();
  std::swap(centerLeds[8].r, centerLeds[8].g);
}

void Light::SetLeftGunLight(CRGB color) {
  fill_solid(leftLeds, 2, color);
}

void Light::SetRightGunLight(CRGB color) {
  fill_solid(rightLeds, 2, color);
}

void Light::SetEyeColor(CRGB color) {
  fill_solid(centerLeds, 9, color);
}

void Light::Update(ulong deltaTime) {
  std::swap(centerLeds[8].r, centerLeds[8].g);
  FastLED.show();
  std::swap(centerLeds[8].r, centerLeds[8].g);
}