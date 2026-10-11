#pragma once

#include "settings/Settings.h"
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <atomic>

// Joins the configured network as station "Portal Turret". When no SSID is
// stored, or joining fails within ConnectTimeout, it starts the "Portal Turret"
// access point with a captive portal (data/www/wifi.html) to pick a network.
class WifiManager {
public:
  // Call after Settings::Initialize and before the web server's static handler is
  // added, so the "/" route below takes priority.
  void Initialize(Settings &settings, AsyncWebServer &server);
  void Update(ulong deltaTime);
  bool IsPortalActive() const { return portalActive; }

private:
  void StartConnecting(const char *ssid, const char *password);
  void StartPortal();
  String ScanJson();
  String PortalUrl() const;

  Settings *settings = nullptr;
  DNSServer dns;
  bool connecting = false;
  ulong connectElapsed = 0;
  std::atomic<bool> portalActive{false};
  std::atomic<bool> rebootRequested{false};
  ulong rebootTimer = 0;
};
