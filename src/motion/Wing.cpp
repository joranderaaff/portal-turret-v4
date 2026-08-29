#include "Wing.h"
#include "Arduino.h"
#include "pins.h"

#define CENTER_ANGLE 90

Wing::Wing(int servoPinIn, int gunServoPinIn, int hallSensorPinIn)
    : gun(gunServoPinIn)
{
  servoPin = servoPinIn;
  hallSensorPin = hallSensorPinIn;
}

void Wing::Initialize()
{
  servo.setPeriodHertz(50); // standard 50 hz servo
  servo.attach(servoPin, 500, 2400);
  Serial.print("Connecting wing servo to pin ");
  Serial.print(servoPin);
  Serial.print("\n");
  servo.write(90);
  gun.Initialize();
}

void Wing::Open()
{
  isOpening = true;
  isClosing = false;
  if (servoPin == PIN_WING_LEFT)
    servo.write(CENTER_ANGLE + 80);
  else
    servo.write(CENTER_ANGLE - 80);
}

void Wing::Close()
{
  isOpening = false;
  isClosing = true;
  isOpen = false;
  if (servoPin == PIN_WING_LEFT)
    servo.write(CENTER_ANGLE - 80);
  else
    servo.write(CENTER_ANGLE + 80);
}

Gun &Wing::GetGun()
{
  return gun;
}

void Wing::Update(ulong deltaTime)
{
  uint16_t hallValue = analogRead(hallSensorPin);

  if (isOpening && hallValue >= 3100)
  {
    isOpening = false;
    isOpen = true;
    servo.write(90);
    Close();
    return;
  }

  if (isClosing && hallValue <= 1100)
  {
    isClosing = false;
    servo.write(90);
    Open();
    return;
  }
}

bool Wing::IsOpen() { return isOpen; }