#include "TurretWebServer.h"

#include "settings/Settings.h"

namespace {

constexpr ulong RADAR_SEND_INTERVAL_MS = 50;

const char *TypeName(SettingType type) {
  switch (type) {
  case SettingType::Int:
    return "int";
  case SettingType::Float:
    return "float";
  case SettingType::Bool:
    return "bool";
  case SettingType::Str:
    return "string";
  }
  return "unknown";
}

void AppendEscaped(String &out, const char *text) {
  out += '"';
  for (const char *c = text; *c != '\0'; c++) {
    if (*c == '"' || *c == '\\') {
      out += '\\';
      out += *c;
    } else if (*c == '\n') {
      out += "\\n";
    } else {
      out += *c;
    }
  }
  out += '"';
}

void AppendValue(String &out, SettingType type, const SettingValue &value) {
  char buffer[24];
  switch (type) {
  case SettingType::Int:
    out += String(value.valueInt);
    break;
  case SettingType::Float:
    snprintf(buffer, sizeof(buffer), "%.6g", value.valueFloat);
    out += buffer;
    break;
  case SettingType::Bool:
    out += value.valueBool ? "true" : "false";
    break;
  case SettingType::Str:
    AppendEscaped(out, value.valueString);
    break;
  }
}

bool ParseBool(const char *text) {
  return strcasecmp(text, "true") == 0 || strcasecmp(text, "on") == 0 ||
         strcmp(text, "1") == 0;
}

} // namespace

String ToJson(Settings *settings) {
  String json = "[";
  for (int i = 0; i < SettingId::COUNT; i++) {
    const SettingsEntry &entry = settings->entries[i];
    if (i > 0) {
      json += ',';
    }
    json += "{\"key\":";
    AppendEscaped(json, entry.key);
    json += ",\"label\":";
    AppendEscaped(json, entry.label);
    json += ",\"type\":";
    AppendEscaped(json, TypeName(entry.type));
    json += ",\"value\":";
    AppendValue(json, entry.type, entry.value);
    json += ",\"default\":";
    AppendValue(json, entry.type, entry.defaultValue);
    if (entry.type == SettingType::Int || entry.type == SettingType::Float) {
      json += ",\"min\":";
      AppendValue(json, entry.type, entry.min);
      json += ",\"max\":";
      AppendValue(json, entry.type, entry.max);
    }
    json += '}';
  }
  json += ']';
  return json;
}

TurretWebServer::TurretWebServer() : webServer(80), socket("/ws") {}

void TurretWebServer::Update(ulong deltaTime) {
  socket.cleanupClients();

  timeSinceRadarSend += deltaTime;
  if (timeSinceRadarSend < RADAR_SEND_INTERVAL_MS) {
    return;
  }
  timeSinceRadarSend = 0;

  if (socket.count() > 0) {
    SendRadar();
    SendMotion();
    SendOrientation();
  }
}

void TurretWebServer::SendRadar() {
  uint8_t buffer[RADAR_MESSAGE_BYTES];
  size_t offset = 0;

  auto writeU8 = [&](uint8_t value) { buffer[offset++] = value; };
  auto writeI16 = [&](int16_t value) {
    buffer[offset++] = value & 0xFF;
    buffer[offset++] = (uint16_t)value >> 8;
  };

  writeU8(MESSAGE_RADAR);
  writeU8(TRACK_COUNT);
  for (uint8_t i = 0; i < TRACK_COUNT; i++) {
    const RadarTarget &target = turret->radar.GetTarget(i);
    writeU8(target.id);
    writeU8((target.available ? 0x01 : 0) | (target.isMoving ? 0x02 : 0));
    writeI16(target.x);
    writeI16(target.y);
    writeI16(target.previousX);
    writeI16(target.previousY);
    writeI16(target.speed);
    writeI16((int16_t)target.resolution);
  }

  socket.binaryAll(buffer, offset);
}

void TurretWebServer::SendMotion() {
  uint8_t buffer[MOTION_MESSAGE_BYTES];
  buffer[0] = MESSAGE_MOTION;

  const float values[6] = {turret->motion.GetAcceleration().x,
                           turret->motion.GetAcceleration().y,
                           turret->motion.GetAcceleration().z,
                           turret->motion.GetSmoothAcceleration().x,
                           turret->motion.GetSmoothAcceleration().y,
                           turret->motion.GetSmoothAcceleration().z};
  // ESP32 is little endian, matching the wire format.
  memcpy(buffer + 1, values, sizeof(values));

  socket.binaryAll(buffer, sizeof(buffer));
}

void TurretWebServer::SendOrientation() {
  uint8_t buffer[ORIENTATION_MESSAGE_BYTES];
  buffer[0] = MESSAGE_ORIENTATION;

  const float values[2] = {turret->motion.GetRoll(), turret->motion.GetPitch()};
  memcpy(buffer + 1, values, sizeof(values));

  socket.binaryAll(buffer, sizeof(buffer));
}

void TurretWebServer::Initialize(Turret &turretIn, Settings &settingsIn) {
  settings = &settingsIn;
  turret = &turretIn;

  webServer.addHandler(&socket);

  webServer.serveStatic("/", LittleFS, "/www/").setDefaultFile("index.html");

  webServer.on("/settings", HTTP_GET, [this](AsyncWebServerRequest *request) {
    request->send(200, "application/json", ToJson(settings));
  });

  webServer.on("/enable_multitarget", HTTP_GET, [this](AsyncWebServerRequest *request) {
    turret->radar.EnableMultiTarget();
    request->send(200, "application/json", "{\"status\":\"OK\"}");
  });

  webServer.on("/settings/reset", HTTP_POST, [this](AsyncWebServerRequest *request) {
    settings->ResetToDefaults();
    request->send(200, "application/json", "{\"status\":\"OK\"}");
  });

  webServer.on("/settings", HTTP_POST, [this](AsyncWebServerRequest *request) {
    for (int i = 0; i < SettingId::COUNT; i++) {
      SettingId id = static_cast<SettingId>(i);
      SettingsEntry entry = settings->entries[id];
      if (request->hasParam(entry.key, true)) {
        switch (entry.type) {
        case SettingType::Int:
          settings->Set(id, (int32_t)request->getParam(entry.key, true)->value().toInt());
          break;
        case SettingType::Float:
          settings->Set(id, request->getParam(entry.key, true)->value().toFloat());
          break;
        case SettingType::Bool:
          settings->Set(id, request->getParam(entry.key, true)->value() == "true");
          break;
        case SettingType::Str:
          settings->Set(id, request->getParam(entry.key, true)->value().c_str());
          break;
        }
      }
    }
    request->send(200, "application/json", "{\"status\":\"OK\"}");
  });

  webServer.begin();
}