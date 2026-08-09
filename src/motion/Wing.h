#include "Arduino.h"
#include "Gun.h"
#include <ESP32Servo.h>

class Wing {
public:
  Wing(int servoPin, int gunServoPin);
  void Initialize();
  void Open();
  void Close();
  void Update(ulong deltaTime);
  bool IsOpen();

private:
  bool isOpening;
  bool isClosing;
  bool isOpen;
  int servoPin;
  Servo servo;
  Gun gun;
};