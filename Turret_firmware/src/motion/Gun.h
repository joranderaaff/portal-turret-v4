#pragma once

#include "Arduino.h"
#include <ESP32Servo.h>

class Gun {
public:
  Gun(int servoPin);
  void Initialize();
  void Extend();
  void Retract();

private:
  int servoPin;
  Servo servo;
};