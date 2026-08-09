#include "Light.h"
#include "pins.h"

void Light::Initialize() {
  FastLED.addLeds<WS2812, PIN_NEOPIXEL_CENTER, GRB>(centerLeds, 9);
  FastLED.addLeds<WS2812, PIN_NEOPIXEL_LEFT, GRB>(leftLeds, 2);
  FastLED.addLeds<WS2812, PIN_NEOPIXEL_RIGHT, GRB>(rightLeds, 2);

  for (int i = 0; i < 9; ++i) {
    centerLeds[i] = CRGB::Red;
  }
  FastLED.show();
}

void Light::Update(ulong deltaTime) {}

Light lights;