#include "Motion.h"

void Motion::Initialize() {
  if (!accel.begin()) {
    return;
  }
}

float Motion::GetAccelerationX() {
    return accelerationX;
}

float Motion::GetAccelerationY() {
    return accelerationY;
}

float Motion::GetAccelerationZ() {
    return accelerationZ;
}

void Motion::Update(ulong deltaTime) {
  sensors_event_t event;
  accel.getEvent(&event);

  accelerationX = event.acceleration.x;
  accelerationY = event.acceleration.y;
  accelerationZ = event.acceleration.z;
}