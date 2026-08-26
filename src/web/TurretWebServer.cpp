#include "TurretWebServer.h"

TurretWebServer::TurretWebServer() : webServer(80)
{
}

void TurretWebServer::Initialize()
{
    webServer.on("/", HTTP_GET, [this]()
                 { this->HandleRequestRoot(); });
}

void TurretWebServer::HandleRequestRoot()
{
    webServer.send(200, "text/plain", "Huge Success");
}