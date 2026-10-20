#include "Wing.h"
#include "Arduino.h"
#include "pins.h"

#define CENTER_ANGLE 90
#define SPEED 70

Wing::Wing(Settings &_settings, Light &light, Audio &audio, int servoPinIn, int gunServoPinIn, int hallSensorPinIn) : settings(_settings), gun(_settings, light, audio, gunServoPinIn) {
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
  Close(false);
}

void Wing::Close(bool force) {
  if (!force && !isOpen) {
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

void Wing::Halt() {
  isOpening = false;
  isClosing = false;
  servo.write(CENTER_ANGLE);
  gun.StopFiring();
}

void Wing::SetRaw(int angle) {
  isOpening = false;
  isClosing = false;
  servo.write(angle);
}

Gun &Wing::GetGun() { return gun; }

void Wing::Update(ulong deltaTime) {
  hallValue = analogRead(hallSensorPin);

  gun.Update(deltaTime);

  if (isOpening || isClosing) {
    timeMoving += deltaTime;
    if (timeMoving >= settings.GetInt(SettingId::GunMoveTimeout)) {
      Serial.println("Wing Movement Timeout");
      if (isOpening) {
        isOpen = true;
      }
      isOpening = false;
      isClosing = false;
      servo.write(90);
    }
  }

  if (isOpening && hallValue >= settings.GetInt(SettingId::GunHallMax)) {
    Serial.println("Wing Is Open");
    isOpening = false;
    isOpen = true;
    servo.write(90);
    return;
  }

  if (isClosing && hallValue <= settings.GetInt(SettingId::GunHallMin)) {
    Serial.println("Wing Is Closed");
    isClosing = false;
    servo.write(90);
    return;
  }
}

HallState Wing::GetHallState() const {
  const uint16_t value = hallValue;
  if (value >= settings.GetInt(SettingId::GunHallMax)) {
    return HallState::Open;
  }
  if (value <= settings.GetInt(SettingId::GunHallMin)) {
    return HallState::Closed;
  }
  return HallState::Between;
}

bool Wing::IsOpen() { return isOpen; }
bool Wing::IsClosing() { return isClosing; }