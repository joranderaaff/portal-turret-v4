#include "WiFi.h"
#include "settings/Settings.h"
#include <ESPAsyncWebServer.h>

class TurretWebServer
{
public:
    TurretWebServer();
    void Initialize(Settings &settings);
    AsyncWebServer webServer;

private:
    //void HandleRequestRoot();
    //void HandleRequestSettings();
    Settings *settings;
};