#include "Arduino.h"
#include "Wing.h"
#include <ESP32Servo.h>

class Gantry {
public:
  Gantry();
  void Initialize();
  void Update(ulong deltaTime);
  void OpenWings();
  void CloseWings();

private:
  Wing wingLeft;
  Wing wingRight;
  Servo servoRotateX;
  Servo servoRotateZ;
};
