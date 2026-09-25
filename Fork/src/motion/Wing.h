#pragma once

#include "Arduino.h"
#include "Gun.h"
#include <ESP32Servo.h>

class Wing {
public:
  Wing(int servoPin, int gunServoPin, int hallSensorPin);
  void Initialize();
  void Open();
  void Close();
  void Update(ulong deltaTime);
  bool IsOpen();
  bool IsClosing();
  Gun& GetGun();

private:
  bool isOpening = false;
  bool isClosing = false;
  bool isOpen = false;
  int servoPin;
  int hallSensorPin;
  ulong timeMoving = 0;
  Servo servo;
  Gun gun;
};