#pragma once

#include "helpers/Math.h"
#include "settings/Settings.h"
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <Wire.h>

struct Acceleration {
  float x = 0;
  float y = 0;
  float z = 0;
};

class Motion {
public:
  Motion(Settings &settings);
  void Initialize();
  void Update(ulong deltaTime);
  Acceleration GetAcceleration();
  Acceleration GetSmoothAcceleration();
  // Degrees, derived from gravity. Roll -180..180, pitch -90..90.
  float GetRoll();
  float GetPitch();

private:
  Settings &settings;
  float damping = 1;
  Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
  Acceleration acceleration;
  Acceleration smoothAcceleration;
};