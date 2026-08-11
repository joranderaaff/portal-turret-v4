#include "Turret.h"
#include "pins.h"
#include "states/StateMachine.h"
#include <Arduino.h>

ulong prevTime;

StateMachine stateMachine;
Gantry gantry;
Motion motion;
Radar radar;
Audio audio;
Light light;

void setup() {

  Turret turret{gantry, motion, radar, audio, light};

  Serial.begin(9600);
  delay(1000);
  Serial.println("This is a triumph");

  prevTime = millis();

  gantry.Initialize();
  light.Initialize();
  motion.Initialize();
  radar.Initialize();
  audio.Initialize();

  stateMachine.Initialize(turret);

  stateMachine.GoToState(StateId::Booting);
}

void loop() {
  ulong currentTime = millis();
  ulong deltaTime = currentTime - prevTime;

  prevTime = currentTime;

  gantry.Update(deltaTime);
  light.Update(deltaTime);
  motion.Update(deltaTime);
  radar.Update(deltaTime);
  audio.Update(deltaTime);

  stateMachine.Update(deltaTime);
}