#include "WifiManager.h"

namespace {

const char *DEVICE_NAME = "portal-turret";
const ulong CONNECT_TIMEOUT_MS = 10000;
const ulong REBOOT_DELAY_MS = 1000;

void AppendJsonString(String &out, const String &text) {
  out += '"';
  for (size_t i = 0; i < text.length(); i++) {
    const char c = text[i];
    if (c == '"' || c == '\\') {
      out += '\\';
      out += c;
    } else if ((uint8_t)c < 0x20) {
      out += ' ';
    } else {
      out += c;
    }
  }
  out += '"';
}

} // namespace

String WifiManager::PortalUrl() const {
  return String("http://") + WiFi.softAPIP().toString() + "/wifi.html";
}

void WifiManager::Initialize(Settings &settingsIn, AsyncWebServer &server) {
  settings = &settingsIn;

  server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
    request->redirect(portalActive ? PortalUrl() : String("/index.html"));
  });

  // Everything the static handler does not know: captive portal probes of
  // phones and laptops end up here and get sent to the setup page.
  server.onNotFound([this](AsyncWebServerRequest *request) {
    if (portalActive) {
      request->redirect(PortalUrl());
    } else {
      request->send(404, "text/plain", "Not found");
    }
  });

  server.on("/wifi/scan", HTTP_GET, [this](AsyncWebServerRequest *request) {
    if (request->hasParam("rescan") && WiFi.scanComplete() != WIFI_SCAN_RUNNING) {
      WiFi.scanDelete();
    }
    request->send(200, "application/json", ScanJson());
  });

  server.on("/wifi/save", HTTP_POST, [this](AsyncWebServerRequest *request) {
    if (!request->hasParam("ssid", true) || request->getParam("ssid", true)->value().length() == 0) {
      request->send(400, "application/json", "{\"status\":\"SSID required\"}");
      return;
    }
    settings->Set(SettingId::WifiSsid, request->getParam("ssid", true)->value().c_str());
    settings->Set(SettingId::WifiPassword,
                  request->hasParam("password", true) ? request->getParam("password", true)->value().c_str() : "");
    request->send(200, "application/json", "{\"status\":\"OK\"}");

    // Restart so the new credentials are tried from a clean state.
    rebootTimer = REBOOT_DELAY_MS;
    rebootRequested = true;
  });

  const char *ssid = settings->GetString(SettingId::WifiSsid);
  if (strlen(ssid) == 0) {
    Serial.println("WiFi: no network configured");
    StartPortal();
  } else {
    StartConnecting(ssid, settings->GetString(SettingId::WifiPassword));
  }
}

void WifiManager::StartConnecting(const char *ssid, const char *password) {
  Serial.printf("WiFi: connecting to \"%s\"\n", ssid);
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(DEVICE_NAME);
  WiFi.begin(ssid, password);
  connecting = true;
  connectElapsed = 0;
}

void WifiManager::StartPortal() {
  Serial.println("WiFi: starting access point with captive portal");
  connecting = false;
  WiFi.disconnect(true);
  // The station interface is needed to scan for networks.
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(DEVICE_NAME);
  dns.start(53, "*", WiFi.softAPIP());
  WiFi.scanNetworks(true);
  portalActive = true;
}

// Scans are started once when the portal opens (and on request): scanning
// makes the access point hop channels, which briefly drops connected clients.
String WifiManager::ScanJson() {
  const int count = WiFi.scanComplete();
  if (count == WIFI_SCAN_RUNNING) {
    return "{\"scanning\":true}";
  }
  if (count < 0) {
    WiFi.scanNetworks(true);
    return "{\"scanning\":true}";
  }

  String json = "{\"scanning\":false,\"networks\":[";
  bool first = true;
  for (int i = 0; i < count; i++) {
    const String ssid = WiFi.SSID(i);
    if (ssid.length() == 0) {
      continue;
    }
    // Several access points of one network: only list the strongest.
    bool duplicate = false;
    for (int j = 0; j < count && !duplicate; j++) {
      if (j != i && WiFi.SSID(j) == ssid) {
        duplicate = WiFi.RSSI(j) > WiFi.RSSI(i) || (WiFi.RSSI(j) == WiFi.RSSI(i) && j < i);
      }
    }
    if (duplicate) {
      continue;
    }
    if (!first) {
      json += ',';
    }
    first = false;
    json += "{\"ssid\":";
    AppendJsonString(json, ssid);
    json += ",\"rssi\":";
    json += WiFi.RSSI(i);
    json += ",\"secure\":";
    json += WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "false" : "true";
    json += '}';
  }
  json += "]}";
  return json;
}

void WifiManager::Update(ulong deltaTime) {
  if (rebootRequested) {
    if (rebootTimer > deltaTime) {
      rebootTimer -= deltaTime;
    } else {
      Serial.println("WiFi: restarting with new credentials");
      Serial.flush();
      ESP.restart();
    }
  }

  if (portalActive) {
    dns.processNextRequest();
  }

  if (!connecting) {
    return;
  }

  if (WiFi.status() == WL_CONNECTED) {
    connecting = false;
    Serial.printf("WiFi: connected, IP %s\n", WiFi.localIP().toString().c_str());
    return;
  }

  connectElapsed += deltaTime;
  if (connectElapsed >= CONNECT_TIMEOUT_MS) {
    Serial.println("WiFi: connecting failed");
    StartPortal();
  }
}
