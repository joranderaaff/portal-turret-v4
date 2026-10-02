#pragma once

#include <Arduino.h>
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

class Motion {
public:
  void Initialize();
  void Update(ulong deltaTime);
  float GetAccelerationX();
  float GetAccelerationY();
  float GetAccelerationZ();

private:
  Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
  float accelerationX;
  float accelerationY;
  float accelerationZ;
};