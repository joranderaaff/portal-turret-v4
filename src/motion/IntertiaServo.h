#include <Arduino.h>
#include <ESP32Servo.h>
#include "helpers/Math.h"
class InertiaServo {
public:
  void attach(int pin);
  void write(int angle);
  void write(int angle, bool instant);
  void Update(ulong deltaTime);
  float moveVelocityTowardZero(float velocity, float deceleration, float dt);

private:
  Servo servo;
  float currentAngle = 0.0f; // degrees
  float velocity = 0.0f;     // degrees / second
  float targetAngle = 0.0f;  // degrees
  float acceleration = 1000.0f;
  float deceleration = 1000.0f;
  float maxVelocity = 600.0f;
};