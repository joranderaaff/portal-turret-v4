#pragma once

#include "Arduino.h"
#include "helpers/Math.h"
#include "pins.h"
#include "settings/Settings.h"
#include <ESP32Servo.h>

struct Turret;

class Gun {
public:
  Gun(Turret &turret, int servoPin);
  void Initialize();
  void Extend();
  void Retract();
  void StartFiring();
  void StopFiring();
  void Update(ulong deltaTime);

private:
  Turret &turret;
  int servoPin;
  int shotBrightness;
  Servo servo;
  bool firing = false;
  ulong firingTime = 0;
  int shotDuration = 0;
};