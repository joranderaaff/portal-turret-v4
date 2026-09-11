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
  static const float X_AXIS_GEAR_RATIO = 1.0;
  static const float Z_AXIS_GEAR_RATIO = 1.0;
  static const int ANGLE_OFFSET_X = -15;
  static const int ANGLE_OFFSET_Z = 0;
  
  Wing wingLeft;
  Wing wingRight;
  Servo servoRotateX;
  Servo servoRotateZ;
};
