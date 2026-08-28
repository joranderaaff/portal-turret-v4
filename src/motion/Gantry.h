#pragma once

#include "Arduino.h"
#include "Wing.h"
#include <ESP32Servo.h>

class Gantry {
public:
  Gantry();
  void Initialize();
  void Update(ulong deltaTime);
  void SetRotationX(int angle);
  void SetRotationZ(int angle);
  void OpenWings();
  void CloseWings();
  Wing& GetWingLeft();
  Wing& GetWingRight();

private:
  Wing wingLeft;
  Wing wingRight;
  Servo servoRotateX;
  Servo servoRotateZ;
};
