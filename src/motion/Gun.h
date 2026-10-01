#pragma once

#include "Arduino.h"
#include "helpers/Math.h"
#include "pins.h"
#include "settings/Settings.h"
#include <ESP32Servo.h>

class Gun {
public:
  Gun(Settings &settings, int servoPin);
  void Initialize();
  void Extend();
  void Retract();
  void StartFiring();
  void StopFiring();
  void Update(ulong deltaTime);

private:
  Settings &settings;
  int servoPin;
  Servo servo;
  bool firing = false;
  ulong firingTime = 0;
  int shotDuration = 0;
};