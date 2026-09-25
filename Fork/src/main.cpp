#include "Turret.h"
#include "pins.h"
#include "states/StateMachine.h"
#include "web/Ota.h"
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

ulong prevTime;

TurretWebServer server;
StateMachine stateMachine;
Gantry gantry;
Motion motion;
Radar radar;
Audio audio;
Light light;
Ota ota;
Settings settings;

const char *ssid = "Portal Turret";

Turret turret{gantry, motion, radar, audio, light, server, settings};

void setup() {

  Serial.begin(115200);
  delay(1000);
  Serial.println("This is a triumph");

  WiFi.softAP(ssid);
  
  ota.Initialize(server.webServer);

  prevTime = millis();

  settings.Initialize();

  server.Initialize(settings);
  gantry.Initialize(settings);
  light.Initialize();
  motion.Initialize();
  radar.Initialize();
  audio.Initialize();

  stateMachine.Initialize(turret);

  stateMachine.GoToState(StateId::Idle);
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
  ota.Update(deltaTime);

  stateMachine.Update(deltaTime);
}