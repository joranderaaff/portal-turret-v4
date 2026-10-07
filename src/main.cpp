#include "Turret.h"
#include "pins.h"
#include "states/StateMachine.h"
#include "web/Ota.h"
#include "web/TurretWebServer.h"
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

ulong prevTime;

Settings settings;
TurretWebServer server;
StateMachine stateMachine;
Audio audio(settings);
Light light;
ADXL motion(settings);
Gantry gantry(settings, light, motion);
Radar radar;
Ota ota;
TargetTracker targetTracker(settings, radar);

const char *ssid = "Portal Turret";

Turret turret{gantry, motion, radar, audio, light, settings, targetTracker};

void setup() {

  Serial.begin(115200);
  delay(1000);
  Serial.println("This is a triumph");

  WiFi.softAP(ssid);

  ota.Initialize(server.webServer);

  prevTime = millis();

  settings.Initialize();

  server.Initialize(turret, settings);
  gantry.Initialize();
  light.Initialize();
  motion.Initialize();
  radar.Initialize();
  audio.Initialize();
  targetTracker.Initialize();

  stateMachine.Initialize(turret);

  stateMachine.GoToState(StateId::Idle);
}

void loop() {
  ulong currentTime = millis();
  ulong deltaTime = currentTime - prevTime;

  prevTime = currentTime;

  gantry.Update(deltaTime);
  motion.Update(deltaTime);
  radar.Update(deltaTime);
  ota.Update(deltaTime);
  server.Update(deltaTime);
  targetTracker.Update(deltaTime);
  light.Update(deltaTime);

  stateMachine.Update(deltaTime);
}