#include "Gun.h"
#include "pins.h"

const int EXTEND_ANGLE = 160;

Gun::Gun(int servoPinIn) { servoPin = servoPinIn; }

void Gun::Initialize()
{
  servo.setPeriodHertz(50); // standard 50 hz servo
  servo.attach(servoPin, 500, 2400);
  Retract();
}

void Gun::Extend()
{
  if (servoPin == PIN_GUN_LEFT)
  {
    servo.write(EXTEND_ANGLE);
  }
  else
  {
    servo.write(180 - EXTEND_ANGLE);
  }
}

void Gun::Retract()
{
  servo.write(90);
}