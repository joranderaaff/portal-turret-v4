#include "Wing.h"
#include "Arduino.h"
#include "pins.h"

#define CENTER_ANGLE 90
#define SPEED 45

Wing::Wing(int servoPinIn, int gunServoPinIn, int hallSensorPinIn)
    : gun(gunServoPinIn) {
  servoPin = servoPinIn;
  hallSensorPin = hallSensorPinIn;
}

void Wing::Initialize() {
  servo.setPeriodHertz(50); // standard 50 hz servo
  servo.attach(servoPin, 500, 2400);
  Serial.print("Connecting wing servo to pin ");
  Serial.print(servoPin);
  Serial.print("\n");
  servo.write(90);
  gun.Initialize();
}

void Wing::Open() {
  if (isOpen) {
    return;
  }
  Serial.println("Opening Wing");
  isOpening = true;
  isClosing = false;
  timeMoving = 0;
  if (servoPin == PIN_WING_LEFT)
    servo.write(CENTER_ANGLE + SPEED);
  else
    servo.write(CENTER_ANGLE - SPEED);
}

void Wing::Close() {
  if (!isOpen) {
    return;
  }
  Serial.println("Closing Wing");
  isOpening = false;
  isClosing = true;
  isOpen = false;
  timeMoving = 0;
  if (servoPin == PIN_WING_LEFT)
    servo.write(CENTER_ANGLE - SPEED);
  else
    servo.write(CENTER_ANGLE + SPEED);
}

Gun &Wing::GetGun() { return gun; }

void Wing::Update(ulong deltaTime) {
  uint16_t hallValue = analogRead(hallSensorPin);

  if (isOpening || isClosing) {
    timeMoving += deltaTime;
    if (timeMoving >= 2000) {
      Serial.println("Wing Movement Timeout");
      if (isOpening) {
        isOpen = true;
      }
      isOpening = false;
      isClosing = false;
      servo.write(90);
    }
  }

  if (isOpening && hallValue >= 2500) {
    Serial.println("Wing Is Open");
    isOpening = false;
    isOpen = true;
    servo.write(90);
    return;
  }

  if (isClosing && hallValue <= 1500) {
    Serial.println("Wing Is Closed");
    isClosing = false;
    servo.write(90);
    return;
  }
}

bool Wing::IsOpen() { return isOpen; }
bool Wing::IsClosing() { return isClosing; }