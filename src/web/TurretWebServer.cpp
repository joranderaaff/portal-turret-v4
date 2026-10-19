#include "TurretWebServer.h"

#include "settings/Settings.h"

namespace {

constexpr ulong RADAR_SEND_INTERVAL_MS = 50;
constexpr ulong HALL_RESEND_INTERVAL_MS = 1000;

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

struct NamedId {
  const char *name;
  int id;
};

const NamedId SERVOS[] = {
    {"rotate_x", (int)GantryServo::RotateX},   {"rotate_z", (int)GantryServo::RotateZ},
    {"wing_left", (int)GantryServo::WingLeft}, {"wing_right", (int)GantryServo::WingRight},
    {"gun_left", (int)GantryServo::GunLeft},   {"gun_right", (int)GantryServo::GunRight},
};

const NamedId AUDIO[] = {
    {"activate", (int)AudioId::Activate}, {"searching", (int)AudioId::Searching},
    {"pickup", (int)AudioId::Pickup},     {"tipped", (int)AudioId::Tipped},
    {"retire", (int)AudioId::Retire},     {"alarm", (int)AudioId::Alarm},
};

// Looks up the id for a POST parameter holding one of the names, -1 if missing or unknown.
template <size_t N> int FindId(AsyncWebServerRequest *request, const char *param, const NamedId (&table)[N]) {
  if (!request->hasParam(param, true)) {
    return -1;
  }
  const String name = request->getParam(param, true)->value();
  for (const NamedId &entry : table) {
    if (name == entry.name) {
      return entry.id;
    }
  }
  return -1;
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
  timeSinceHallSend += deltaTime;
  if (timeSinceRadarSend < RADAR_SEND_INTERVAL_MS) {
    return;
  }
  timeSinceRadarSend = 0;

  if (socket.count() > 0) {
    SendRadar();
    SendMotion();
    SendOrientation();
    SendHall();
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

void TurretWebServer::SendHall() {
  uint8_t buffer[HALL_MESSAGE_BYTES];
  size_t offset = 0;
  buffer[offset++] = MESSAGE_HALL;
  for (const Wing *wing : {&turret->gantry.GetWingLeft(), &turret->gantry.GetWingRight()}) {
    const uint16_t value = wing->GetHallValue();
    buffer[offset++] = value & 0xFF;
    buffer[offset++] = value >> 8;
    buffer[offset++] = (uint8_t)wing->GetHallState();
  }

  // Only send changes, plus a slow resend so a client that just connected gets the current state.
  if (memcmp(buffer, lastHallMessage, sizeof(buffer)) == 0 && timeSinceHallSend < HALL_RESEND_INTERVAL_MS) {
    return;
  }
  memcpy(lastHallMessage, buffer, sizeof(buffer));
  timeSinceHallSend = 0;
  socket.binaryAll(buffer, sizeof(buffer));
}

void TurretWebServer::SendLog(LogLevel level, const char *message) {
  if (socket.count() == 0) {
    return;
  }
  const size_t length = strlen(message);
  uint8_t buffer[2 + length];
  buffer[0] = MESSAGE_LOG;
  buffer[1] = (uint8_t)level;
  memcpy(buffer + 2, message, length);
  socket.binaryAll(buffer, sizeof(buffer));
}

void TurretWebServer::QueueDiagnose(AsyncWebServerRequest *request, const DiagnoseCommand &command) {
  if (xQueueSend(diagnoseQueue, &command, 0) != pdTRUE) {
    request->send(503, "application/json", "{\"status\":\"BUSY\"}");
    return;
  }
  request->send(200, "application/json", "{\"status\":\"OK\"}");
}

void TurretWebServer::Initialize(Turret &turretIn, Settings &settingsIn) {
  settings = &settingsIn;
  turret = &turretIn;
  diagnoseQueue = xQueueCreate(8, sizeof(DiagnoseCommand));

  webServer.addHandler(&socket);

  webServer.serveStatic("/", LittleFS, "/www/").setDefaultFile("index.html");

  webServer.on("/settings", HTTP_GET, [this](AsyncWebServerRequest *request) {
    request->send(200, "application/json", ToJson(settings));
  });

  webServer.on("/enable_multitarget", HTTP_GET, [this](AsyncWebServerRequest *request) {
    turret->radar.EnableMultiTarget();
    request->send(200, "application/json", "{\"status\":\"OK\"}");
  });

  webServer.on("/start", HTTP_POST, [this](AsyncWebServerRequest *request) {
    startRequested = true;
    request->send(200, "application/json", "{\"status\":\"OK\"}");
  });

  // Diagnose actions. The main loop runs them and switches the turret to manual mode.
  webServer.on("/diagnose/servo", HTTP_POST, [this](AsyncWebServerRequest *request) {
    const int id = FindId(request, "servo", SERVOS);
    if (id < 0 || !request->hasParam("value", true)) {
      request->send(400, "application/json", "{\"status\":\"BAD_REQUEST\"}");
      return;
    }
    const int value = request->getParam("value", true)->value().toInt();
    if (value < 0 || value > 180) {
      request->send(400, "application/json", "{\"status\":\"BAD_REQUEST\"}");
      return;
    }
    QueueDiagnose(request, {DiagnoseCommandType::SetServo, id, value});
  });

  webServer.on("/diagnose/leds", HTTP_POST, [this](AsyncWebServerRequest *request) {
    QueueDiagnose(request, {DiagnoseCommandType::LedTest, 0, 0});
  });

  webServer.on("/diagnose/audio", HTTP_POST, [this](AsyncWebServerRequest *request) {
    const int id = FindId(request, "audio", AUDIO);
    if (id < 0) {
      request->send(400, "application/json", "{\"status\":\"BAD_REQUEST\"}");
      return;
    }
    QueueDiagnose(request, {DiagnoseCommandType::PlayAudio, id, 0});
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