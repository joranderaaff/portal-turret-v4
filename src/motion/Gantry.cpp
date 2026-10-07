#include "motion/Gantry.h"

static const float kGravity = 9.81f;
static const float kTippedMinZ = 0.5f * kGravity;

constexpr ulong IMPULSE_DETECTION_UPDATE_INTERVAL = 50;

Gantry::Gantry(Settings &_settings, Light &light, ADXL &_motion) : settings(_settings), motion(_motion), wingLeft(_settings, light, PIN_WING_LEFT, PIN_GUN_LEFT, PIN_HALL_LEFT), wingRight(_settings, light, PIN_WING_RIGHT, PIN_GUN_RIGHT, PIN_HALL_RIGHT) {
}

bool Gantry::IsTippedOver() {
  //return motion.GetSmoothAcceleration().z < kTippedMinZ;
  return false;
}

bool Gantry::IsPickedUp() {
  return pickupImpulseDetected;
}

bool Gantry::IsAtRest() {
  return false;
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
  
  timeSinceLastImpulseUpdate += deltaTime;
  if(timeSinceLastImpulseUpdate > IMPULSE_DETECTION_UPDATE_INTERVAL) {
    timeSinceLastImpulseUpdate -= IMPULSE_DETECTION_UPDATE_INTERVAL;
    pickupImpulseDetected = impulseDetection.AddSample(motion.GetSmoothAcceleration().z);
  }
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