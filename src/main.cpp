#include <Arduino.h>
#include "pins.h"
#include "states/StateMachine.h"
#include "light/Light.h"
#include "motion/Gantry.h"
#include "sensors/Motion.h"
#include "sensors/Radar.h"

StateMachine stateMachine;
Gantry gantry;

ulong prevTime;

void setup() {
    prevTime = millis();
    gantry.Initialize();

    stateMachine.GoToState(StateId::Booting);
}

void loop() {
    ulong currentTime = millis();
    ulong deltaTime = currentTime - prevTime;
    prevTime = currentTime;

    stateMachine.Update(deltaTime);
}