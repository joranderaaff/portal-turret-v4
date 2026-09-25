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

void Light::Update(ulong deltaTime) {

  uint8_t t = millis() / 4;
  uint8_t tri = triwave8(t);

  fill_solid(leftLeds, 2, HeatColor(tri));
  fill_solid(rightLeds, 2, HeatColor(tri));
  fill_solid(centerLeds, 9, CRGB::Red);
  
  std::swap(centerLeds[8].r, centerLeds[8].g);
  FastLED.show();
  std::swap(centerLeds[8].r, centerLeds[8].g);
}