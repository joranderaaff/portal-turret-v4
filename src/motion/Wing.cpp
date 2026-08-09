#include "Wing.h"
#include "Arduino.h"
#include "pins.h"

Wing::Wing(int servoPinIn, int gunServoPin) : gun(gunServoPin) {
  servoPin = servoPinIn;
}

void Wing::Initialize() {
  servo.setPeriodHertz(50); // standard 50 hz servo
  servo.attach(servoPin, 500, 2400);

  gun.Initialize();
}

void Wing::Open() {
  isOpening = true;
  isClosing = false;
  servo.write(0);
}

void Wing::Close() {
  isOpening = false;
  isClosing = true;
  isOpen = false;
  servo.write(180);
}

void Wing::Update(ulong deltaTime) {
  uint16_t hallValue = analogRead(PIN_HALL_LEFT);

  if (isOpening && hallValue >= 4096) {
    isOpening = false;
    isOpen = true;
    servo.write(90);
  }

  if (isClosing && hallValue <= 600) {
    isClosing = false;
    servo.write(90);
  }
}

bool Wing::IsOpen() {
  return isOpen;
}