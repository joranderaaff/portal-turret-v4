#pragma once

#include "Arduino.h"
#include "Wing.h"
#include "helpers/Math.h"
#include "pins.h"
#include "light/Light.h"
#include "sensors/Motion.h"
#include "settings/Settings.h"
#include "motion/IntertiaServo.h"

class Gantry {
public:
  Gantry(Settings &settings, Light &light, Motion &motion);
  void Initialize();
  void Update(ulong deltaTime);
  void SetRotationX(float angle, bool force);
  void SetRotationZ(float angle, bool force);
  void OpenWings();
  void CloseWings();
  Wing &GetWingLeft();
  Wing &GetWingRight();
  // Level checks on the smoothed acceleration (no hysteresis yet).
  bool IsTippedOver();
  bool IsPickedUp();

private:
  float X_AXIS_GEAR_RATIO = 75.0 / 17.0;
  float Z_AXIS_GEAR_RATIO = 30.0 / 15.0;
  float angleOffsetX = 0;
  float angleOffsetZ = 0;
  float maxAngleX = 90;
  float maxAngleZ = 90;

  float currentAngleX;
  float targetAngleX;

  float currentAngleZ;
  float targetAngleZ;

  Wing wingLeft;
  Wing wingRight;
  InertiaServo servoRotateX;
  InertiaServo servoRotateZ;
  Settings &settings;
  Motion &motion;
};
