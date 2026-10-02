#include "Motion.h"

void Motion::Initialize() {
  /* Initialise the sensor */
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

  /* Display the results (acceleration is measured in m/s^2) */
  // Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print("  ");
  // Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print("  ");
  // Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print("  ");Serial.println("m/s^2 ");
  // delay(500);
}