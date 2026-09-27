#include "WiFi.h"
#include "settings/Settings.h"
#include <ESPAsyncWebServer.h>
#include <Turret.h>

class TurretWebServer {
public:
  TurretWebServer();
  void Initialize(Turret &turret, Settings &settings);
  AsyncWebServer webServer;

private:
  Settings *settings;
  Turret *turret;
};