#include "motion/Gantry.h"

// TODO: tune thresholds, add hysteresis / hold time.
static const float kGravity = 9.81f;
static const float kTippedMinZ = 0.5f * kGravity; // upright reads ~+1 g on Z
static const float kPickedUpDeviation = 0.3f * kGravity;

Gantry::Gantry(Settings &_settings, Light &light, Motion &_motion) : settings(_settings), motion(_motion), wingLeft(_settings, light, PIN_WING_LEFT, PIN_GUN_LEFT, PIN_HALL_LEFT), wingRight(_settings, light, PIN_WING_RIGHT, PIN_GUN_RIGHT, PIN_HALL_RIGHT) {
}

bool Gantry::IsTippedOver() {
  return motion.GetSmoothAcceleration().z < kTippedMinZ;
}

bool Gantry::IsPickedUp() {
  Acceleration a = motion.GetSmoothAcceleration();
  float magnitude = sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
  return fabsf(magnitude - kGravity) > kPickedUpDeviation;
}

void Gantry::Initialize() {
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  angleOffsetX = settings.GetFloat(SettingId::AngleOffsetX);
  angleOffsetZ = settings.GetFloat(SettingId::AngleOffsetZ);
  maxAngleX = settings.GetFloat(SettingId::AngleMaxX);
  maxAngleZ = settings.GetFloat(SettingId::AngleMaxZ);

  delay(100);

  servoRotateX.attach(PIN_ROTATE_X);
  SetRotationX(0, true);

  servoRotateZ.attach(PIN_ROTATE_Z);
  SetRotationZ(0, true);

  wingLeft.Initialize();
  wingRight.Initialize();

  wingLeft.Close();
  wingRight.Close();
}

void Gantry::SetRotationX(float angle, bool force) {
  angle = constrain(angle, -maxAngleX, maxAngleX);
  targetAngleX = 90 + (angle + angleOffsetX) * X_AXIS_GEAR_RATIO;
  servoRotateX.write(targetAngleX, force);
}

void Gantry::SetRotationZ(float angle, bool force) {
  angle = constrain(angle, -maxAngleZ, maxAngleZ);
  targetAngleZ = 90 - (angle + angleOffsetZ) * Z_AXIS_GEAR_RATIO;
  servoRotateZ.write(targetAngleZ, force);

}

void Gantry::Update(ulong deltaTime) {
  servoRotateX.Update(deltaTime);
  servoRotateZ.Update(deltaTime);

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