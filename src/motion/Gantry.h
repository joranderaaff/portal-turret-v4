#pragma once

#include "Arduino.h"
#include "Wing.h"
#include "settings/Settings.h"
#include <ESP32Servo.h>

class Gantry {
public:
  Gantry();
  void Initialize(Settings &settings);
  void Update(ulong deltaTime);
  void SetRotationX(int angle, bool force);
  void SetRotationZ(int angle, bool force);
  void OpenWings();
  void CloseWings();
  Wing& GetWingLeft();
  Wing& GetWingRight();

private:
  float X_AXIS_GEAR_RATIO = 75.0/17.0;
  float Z_AXIS_GEAR_RATIO = 30.0/15.0;
  float ANGLE_OFFSET_X = -3.4;
  float ANGLE_OFFSET_Z = 0;
  
  Wing wingLeft;
  Wing wingRight;
  Servo servoRotateX;
  Servo servoRotateZ;
  Settings *settings;
};
