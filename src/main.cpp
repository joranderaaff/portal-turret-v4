#include "Turret.h"
#include "pins.h"
#include "states/StateMachine.h"
#include "web/Ota.h"
#include "web/TurretWebServer.h"
#include "web/WifiManager.h"
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

ulong prevTime;

Settings settings;
TurretWebServer server;
Logger logger(server);
StateMachine stateMachine;
Audio audio(settings);
Light light;
ADXL motion(settings);
Gantry gantry(settings, light, audio, motion);
Radar radar;
Ota ota;
WifiManager wifi;
TargetTracker targetTracker(settings, radar);

Turret turret{gantry, motion, radar, audio, light, settings, targetTracker, logger};

void setup() {

  Serial.begin(115200);
  delay(1000);
  Serial.println("This is a triumph");

  settings.Initialize();

  ota.Initialize(server.webServer);
  wifi.Initialize(settings, server.webServer);

  prevTime = millis();

  server.Initialize(turret, settings);
  gantry.Initialize();
  light.Initialize();
  motion.Initialize();
  radar.Initialize();
  audio.Initialize();
  targetTracker.Initialize();

  stateMachine.Initialize(turret);

  stateMachine.GoToState(StateId::Booting);
}

void loop() {
  ulong currentTime = millis();
  ulong deltaTime = currentTime - prevTime;

  prevTime = currentTime;

  gantry.Update(deltaTime);
  motion.Update(deltaTime);
  radar.Update(deltaTime);
  ota.Update(deltaTime);
  wifi.Update(deltaTime);
  server.Update(deltaTime);
  targetTracker.Update(deltaTime);
  light.Update(deltaTime);

  if (server.ConsumeStartRequest()) {
    stateMachine.RequestStart();
  }
  DiagnoseCommand diagnoseCommand;
  while (server.ConsumeDiagnoseCommand(diagnoseCommand)) {
    stateMachine.RequestDiagnose(diagnoseCommand);
  }
  stateMachine.Update(deltaTime);
}