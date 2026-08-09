#pragma once

#include "Arduino.h"
#include "driver/i2s.h"
#include "pins.h"
#include <math.h>

class Audio {
public:
  void Initialize();
  void Update(ulong deltaTime);
  void PlaySound();
  void LoopSound();
};