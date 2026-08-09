#include "Motion.h"

void Motion::Initialize()
{
    /* Initialise the sensor */
    if (!accel.begin())
    {
        return;
    }
}

void Motion::Update(ulong deltaTime)
{
    sensors_event_t event;
    accel.getEvent(&event);

    /* Display the results (acceleration is measured in m/s^2) */
    // Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print("  ");
    // Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print("  ");
    // Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print("  ");Serial.println("m/s^2 ");
    // delay(500);
}