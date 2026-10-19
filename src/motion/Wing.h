#pragma once

#include "Arduino.h"
#include "Gun.h"
#include "settings/Settings.h"
#include <ESP32Servo.h>

// What the hall sensor reads against the GunHallMin/GunHallMax settings.
enum class HallState : uint8_t {
  Closed,  // at or below GunHallMin
  Between,
  Open     // at or above GunHallMax
};

class Wing {
public:
  Wing(Settings &settings, Light &light, Audio &audio, int servoPin, int gunServoPin, int hallSensorPin);
  void Initialize();
  void Open();
  void Close();
  void Update(ulong deltaTime);
  // Stops any movement and the gun, leaving the wing where it is.
  void Halt();
  // Writes the servo directly (0..180), cancelling any open/close movement.
  void SetRaw(int angle);
  bool IsOpen();
  bool IsClosing();
  Gun &GetGun();
  uint16_t GetHallValue() const { return hallValue; }
  HallState GetHallState() const;

private:
  Settings &settings;
  bool isOpening = false;
  bool isClosing = false;
  bool isOpen = false;
  int servoPin;
  int hallSensorPin;
  ulong timeMoving = 0;
  volatile uint16_t hallValue = 0;
  Servo servo;
  Gun gun;
};