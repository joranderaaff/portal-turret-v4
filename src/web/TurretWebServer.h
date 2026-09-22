#include "WiFi.h"
#include "WebServer.h"
#include "settings/Settings.h"

class TurretWebServer
{
public:
    TurretWebServer();
    void Initialize(Settings &settings);

private:
    void HandleRequestRoot();
    void HandleRequestSettings();
    WebServer webServer;
    Settings *settings;
};