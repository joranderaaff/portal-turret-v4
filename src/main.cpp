#include "pins.h"
#include "states/StateMachine.h"
#include <Arduino.h>
#include "motion/Gantry.h"

StateMachine stateMachine;
Gantry gantry;

ulong prevTime;

void setup() {
    prevTime = millis();
    gantry.Initialize();
}

void loop() {
    ulong currentTime = millis();
    ulong deltaTime = currentTime - prevTime;
    prevTime = currentTime;

    stateMachine.Update(deltaTime);
}