#include "WiFi.h"
#include "WebServer.h"

class TurretWebServer
{
public:
    TurretWebServer();
    void Initialize();

private:
    void HandleRequestRoot();
    WebServer webServer;
};