#include "Gun.h"

Gun::Gun(int servoPinIn) { servoPin = servoPinIn; }

void Gun::Initialize() {
  servo.setPeriodHertz(50); // standard 50 hz servo
  servo.attach(servoPin, 500, 2400);
}