#include "ADXL.h"

ADXL::ADXL(Settings &_settings) : settings(_settings) {}

void ADXL::Initialize() {
  damping = settings.GetFloat(SettingId::AccelerationDamping);
  if (!accel.begin()) {
    return;
  }
}

Acceleration ADXL::GetAcceleration() {
  return acceleration;
}

Acceleration ADXL::GetSmoothAcceleration() {
  return smoothAcceleration;
}

float ADXL::GetRoll() {
  return atan2f(acceleration.y, acceleration.z) * RAD_TO_DEG;
}

float ADXL::GetPitch() {
  return atan2f(-acceleration.x, sqrtf(acceleration.y * acceleration.y + acceleration.z * acceleration.z)) * RAD_TO_DEG;
}

void ADXL::Update(ulong deltaTime) {
  sensors_event_t event;
  if (accel.getEvent(&event)) {
    //ADXL345 is mounted sideways. Interpret values like this:
    acceleration.x = event.acceleration.z;
    acceleration.y = event.acceleration.x;
    acceleration.z = event.acceleration.y;

  }
  
  float deltaTimeSeconds = (float)deltaTime / 1000.0f;
  smoothAcceleration.x = Damp(smoothAcceleration.x, acceleration.x, damping, deltaTimeSeconds);
  smoothAcceleration.y = Damp(smoothAcceleration.y, acceleration.y, damping, deltaTimeSeconds);
  smoothAcceleration.z = Damp(smoothAcceleration.z, acceleration.z, damping, deltaTimeSeconds);
}