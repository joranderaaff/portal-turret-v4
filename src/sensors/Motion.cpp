#include "Motion.h"
#include "Turret.h"

Motion::Motion(Turret &_turret) : turret(_turret) {}

void Motion::Initialize() {
  damping = turret.settings.GetFloat(SettingId::AccelerationDamping);
  if (!accel.begin()) {
    return;
  }
}

Acceleration Motion::GetAcceleration() {
  return acceleration;
}

Acceleration Motion::GetSmoothAcceleration() {
  return smoothAcceleration;
}

float Motion::GetRoll() {
  return atan2f(acceleration.y, acceleration.z) * RAD_TO_DEG;
}

float Motion::GetPitch() {
  return atan2f(-acceleration.x, sqrtf(acceleration.y * acceleration.y + acceleration.z * acceleration.z)) * RAD_TO_DEG;
}

void Motion::Update(ulong deltaTime) {
  sensors_event_t event;
  if (accel.getEvent(&event)) {
    acceleration.x = event.acceleration.x;
    acceleration.y = event.acceleration.y;
    acceleration.z = event.acceleration.z;
  }
  float deltaTimeSeconds = (float)deltaTime / 1000.0f;
  smoothAcceleration.x = Damp(smoothAcceleration.x, acceleration.x, damping, deltaTimeSeconds);
  smoothAcceleration.y = Damp(smoothAcceleration.y, acceleration.y, damping, deltaTimeSeconds);
  smoothAcceleration.z = Damp(smoothAcceleration.z, acceleration.z, damping, deltaTimeSeconds);
}