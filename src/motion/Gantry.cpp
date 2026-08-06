#include "motion/Gantry.h"
#include "../pins.h"
#include "Arduino.h"

Gantry::Gantry() : wingLeft(PIN_WING_LEFT, PIN_GUN_LEFT), wingRight(PIN_WING_RIGHT, PIN_GUN_RIGHT) {}

void Gantry::Initialize() {
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  wingLeft.Initialize();
  wingRight.Initialize();
}

void Gantry::Update(ulong deltaTime) {
  wingLeft.Update(deltaTime);
  wingRight.Update(deltaTime);
}

void Gantry::OpenWings() {
  wingLeft.Open();
  wingRight.Open();
}

void Gantry::CloseWings() {
  wingLeft.Close();
  wingRight.Close();
}