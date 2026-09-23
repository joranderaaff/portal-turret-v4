#include "TurretWebServer.h"

#include "settings/Settings.h"

namespace {

const char* TypeName(SettingType type) {
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

void AppendEscaped(String& out, const char* text) {
  out += '"';
  for (const char* c = text; *c != '\0'; c++) {
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

void AppendValue(String& out, SettingType type, const SettingValue& value) {
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

bool ParseBool(const char* text) {
  return strcasecmp(text, "true") == 0 || strcasecmp(text, "on") == 0 ||
         strcmp(text, "1") == 0;
}

bool SetFromString(Settings settings, SettingId id, const char *text) {
  const SettingsEntry *entry = settings.Get(id);
  if (entry == nullptr) {
    return false;
  }
  switch (entry->type) {
  case SettingType::Int:
    return settings.Set(id, (int32_t)strtol(text, nullptr, 10));
  case SettingType::Float:
    return settings.Set(id, (float)strtod(text, nullptr));
  case SettingType::Bool:
    return settings.Set(id, ParseBool(text));
  case SettingType::Str:
    return settings.Set(id, text);
  }
  return false;
}

}  // namespace

String ToJson(Settings* settings) {
  String json = "[";
  for (int i = 0; i < SettingId::COUNT; i++) {
    const SettingsEntry& entry = settings->entries[i];
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

TurretWebServer::TurretWebServer() : webServer(80) {}

void TurretWebServer::Initialize(Settings& settingsIn) {
  settings = &settingsIn;

  webServer.on("/", HTTP_GET, [this](AsyncWebServerRequest* request) {
    request->send(200, "application/json", "{\"status\":\"OK\"}");
  });
  webServer.on("/settings", HTTP_GET, [this](AsyncWebServerRequest* request) {
    request->send(200, "application/json", ToJson(settings));
  });
  webServer.begin();
}