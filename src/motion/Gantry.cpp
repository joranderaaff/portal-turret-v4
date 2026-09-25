#include "motion/Gantry.h"

#include "Arduino.h"
#include "helpers/Math.h"
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

  angleOffsetX = settings->GetInt(SettingId::AngleOffsetX);
  angleOffsetZ = settings->GetInt(SettingId::AngleOffsetZ);

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

void Gantry::SetRotationX(float angle, bool force) {
  angle = constrain(angle, -30, 30);
  targetAngleX = 90 + (angle + angleOffsetX) * X_AXIS_GEAR_RATIO;
  if (force) {
    currentAngleX = targetAngleX;
    servoRotateX.write(currentAngleX);
  }
}

void Gantry::SetRotationZ(float angle, bool force) {
  angle = constrain(angle, -30, 30);
  targetAngleZ = 90 - (angle + angleOffsetZ) * Z_AXIS_GEAR_RATIO;
  if (force) {
    currentAngleZ = targetAngleZ;
    servoRotateZ.write(currentAngleZ);
  }
}

void Gantry::Update(ulong deltaTime) {
  if (wingLeft.IsOpen() && wingRight.IsOpen()) {
    float deltaTimeSeconds = deltaTime / 1000.0;
    currentAngleX = Damp(currentAngleX, targetAngleX, 5, deltaTimeSeconds);
    servoRotateX.write(currentAngleX);
    currentAngleZ = Damp(currentAngleZ, targetAngleZ, 5, deltaTimeSeconds);
    servoRotateZ.write(currentAngleZ);
  }

  wingLeft.Update(deltaTime);
  wingRight.Update(deltaTime);
}

void Gantry::OpenWings() {
  wingLeft.Open();
  wingRight.Open();
}

Wing &Gantry::GetWingLeft() { return wingLeft; }

Wing &Gantry::GetWingRight() { return wingRight; }

void Gantry::CloseWings() {
  SetRotationX(0, true);
  SetRotationZ(0, true);
  wingLeft.Close();
  wingRight.Close();
}