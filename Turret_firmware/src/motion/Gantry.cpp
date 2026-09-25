#include "motion/Gantry.h"

#include "Arduino.h"
#include "pins.h"

Gantry::Gantry()
    : wingLeft(PIN_WING_LEFT, PIN_GUN_LEFT, PIN_HALL_LEFT),
      wingRight(PIN_WING_RIGHT, PIN_GUN_RIGHT, PIN_HALL_RIGHT) {}

void Gantry::Initialize(Settings &settingsIn) {
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  settings = &settingsIn;

  ANGLE_OFFSET_X = settings->GetInt(SettingId::AngleOffsetX);
  ANGLE_OFFSET_Z = settings->GetInt(SettingId::AngleOffsetZ);

  delay(100);

  servoRotateX.setPeriodHertz(50);
  servoRotateX.attach(PIN_ROTATE_X, 500, 2400);
  SetRotationX(0, true);

  servoRotateZ.setPeriodHertz(50);
  servoRotateZ.attach(PIN_ROTATE_Z, 500, 2400);
  SetRotationZ(0, true);

  wingLeft.Initialize();
  wingRight.Initialize();

  wingLeft.Close();
  wingRight.Close();
}

void Gantry::SetRotationX(int angle, bool force) {
  if (force || wingLeft.IsOpen() && wingRight.IsOpen()) {
    servoRotateX.write(round(90 + (angle + ANGLE_OFFSET_X) * X_AXIS_GEAR_RATIO));
  }
}

void Gantry::SetRotationZ(int angle, bool force) {
  if (force || wingLeft.IsOpen() && wingRight.IsOpen()) {
    servoRotateZ.write(round(90 + (angle +  ANGLE_OFFSET_Z) * Z_AXIS_GEAR_RATIO));
  }
}

void Gantry::Update(ulong deltaTime) {
  wingLeft.Update(deltaTime);
  wingRight.Update(deltaTime);
}

void Gantry::OpenWings() {
  wingLeft.Open();
  wingRight.Open();
}

Wing& Gantry::GetWingLeft() { return wingLeft; }

Wing& Gantry::GetWingRight() { return wingRight; }

void Gantry::CloseWings() {
  SetRotationX(0, true);
  SetRotationZ(0, true);
  wingLeft.Close();
  wingRight.Close();
}