#pragma once

#include "Arduino.h"
#include "Gun.h"
#include "settings/Settings.h"
#include <ESP32Servo.h>

class Wing {
public:
  Wing(Settings &settings, Light &light, Audio &audio, int servoPin, int gunServoPin, int hallSensorPin);
  void Initialize();
  void Open();
  void Close();
  void Update(ulong deltaTime);
  // Stops any movement and the gun, leaving the wing where it is.
  void Halt();
  // Writes the servo directly (0..180), cancelling any open/close movement.
  void SetRaw(int angle);
  bool IsOpen();
  bool IsClosing();
  Gun &GetGun();

private:
  Settings &settings;
  bool isOpening = false;
  bool isClosing = false;
  bool isOpen = false;
  int servoPin;
  int hallSensorPin;
  ulong timeMoving = 0;
  Servo servo;
  Gun gun;
};