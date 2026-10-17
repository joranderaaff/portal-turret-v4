#pragma once

#include "Arduino.h"
#include "audio/Audio.h"
#include "helpers/Math.h"
#include "pins.h"
#include "light/Light.h"
#include "settings/Settings.h"
#include <ESP32Servo.h>

class Gun {
public:
  Gun(Settings &settings, Light &light, Audio &audio, int servoPin);
  void Initialize();
  void Extend();
  void Retract();
  void StartFiring();
  void StopFiring();
  // Writes the servo directly (0..180), stopping any firing animation.
  void SetRaw(int angle);
  void Update(ulong deltaTime);

private:
  Settings &settings;
  Light &light;
  Audio &audio;
  int servoPin;
  int shotBrightness = 0;
  int heatBrightness = 0;
  Servo servo;
  bool firing = false;
  ulong firingTime = 0;
  int shotDuration = 0;
  float barrelCooldownDuration = 1.0f;
  float barrelHeatupDuration = 1.0f;
  float barrelHeat = 0;
};