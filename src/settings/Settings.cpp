#include "Settings.h"

namespace {

const char *NVS_NAMESPACE = "turret";
const size_t NVS_KEY_MAX = 15;

const char *TypeName(SettingType type) {
  switch (type) {
    case SettingType::Int: return "int";
    case SettingType::Float: return "float";
    case SettingType::Bool: return "bool";
    case SettingType::Str: return "string";
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

}  // namespace

SettingValue::SettingValue(const char *v) {
  strlcpy(valueString, v, SETTING_STRING_MAX);
}

SettingsEntry::SettingsEntry(const char *key, const char *label,
                             const char *group, SettingType type,
                             SettingValue defaultValue, SettingValue min,
                             SettingValue max)
    : key(key),
      label(label),
      group(group),
      type(type),
      value(defaultValue),
      defaultValue(defaultValue),
      min(min),
      max(max) {}

SettingsEntry::SettingsEntry(const char *key, const char *label,
                             const char *group, SettingType type,
                             SettingValue defaultValue)
    : key(key),
      label(label),
      group(group),
      type(type),
      value(defaultValue),
      defaultValue(defaultValue),
      min((int32_t)0),
      max((int32_t)0) {}

// The rows must stay in the same order as SettingId.
Settings::Settings()
    : entries{
          {"AngleOffsetX", "Angle offset X", "Motion", SettingType::Int,
           (int32_t)0, (int32_t)-90, (int32_t)90},
          {"AngleOffsetY", "Angle offset Y", "Motion", SettingType::Int,
           (int32_t)0, (int32_t)-90, (int32_t)90},
          {"Test", "Test", "Debug", SettingType::Float, 10.5f, -90.0f, 90.0f},
      },
      prefsReady(false) {}

void Settings::Initialize() {
  for (int i = 0; i < SettingId::COUNT; i++) {
    if (strlen(entries[i].key) > NVS_KEY_MAX) {
      Serial.print("Settings: key too long for NVS: ");
      Serial.println(entries[i].key);
    }
  }

  prefsReady = prefs.begin(NVS_NAMESPACE, false);
  if (!prefsReady) {
    Serial.println("Settings: could not open NVS, using defaults");
    return;
  }
  Load();
}

void Settings::Load() {
  for (int i = 0; i < SettingId::COUNT; i++) {
    SettingsEntry &entry = entries[i];
    if (!prefs.isKey(entry.key)) {
      continue;
    }
    switch (entry.type) {
      case SettingType::Int:
        entry.value.valueInt = prefs.getInt(entry.key, entry.value.valueInt);
        break;
      case SettingType::Float:
        entry.value.valueFloat =
            prefs.getFloat(entry.key, entry.value.valueFloat);
        break;
      case SettingType::Bool:
        entry.value.valueBool = prefs.getBool(entry.key, entry.value.valueBool);
        break;
      case SettingType::Str:
        prefs.getString(entry.key, entry.value.valueString,
                        SETTING_STRING_MAX);
        break;
    }
  }
}

void Settings::Persist(const SettingsEntry &entry) {
  if (!prefsReady) {
    return;
  }
  switch (entry.type) {
    case SettingType::Int:
      prefs.putInt(entry.key, entry.value.valueInt);
      break;
    case SettingType::Float:
      prefs.putFloat(entry.key, entry.value.valueFloat);
      break;
    case SettingType::Bool:
      prefs.putBool(entry.key, entry.value.valueBool);
      break;
    case SettingType::Str:
      prefs.putString(entry.key, entry.value.valueString);
      break;
  }
}

const SettingsEntry *Settings::Get(SettingId id) const {
  if (id < 0 || id >= SettingId::COUNT) {
    Serial.print("Settings: unknown id ");
    Serial.println((int)id);
    return nullptr;
  }
  return &entries[id];
}

SettingsEntry *Settings::Get(SettingId id) {
  return const_cast<SettingsEntry *>(
      static_cast<const Settings *>(this)->Get(id));
}

int32_t Settings::GetInt(SettingId id) const {
  const SettingsEntry *entry = Get(id);
  if (entry == nullptr) {
    return 0;
  }
  if (entry->type != SettingType::Int) {
    Serial.print("Settings: not an int: ");
    Serial.println(entry->key);
    return 0;
  }
  return entry->value.valueInt;
}

float Settings::GetFloat(SettingId id) const {
  const SettingsEntry *entry = Get(id);
  if (entry == nullptr) {
    return 0.0f;
  }
  // Ints read fine as floats; anything else does not.
  if (entry->type == SettingType::Int) {
    return (float)entry->value.valueInt;
  }
  if (entry->type != SettingType::Float) {
    Serial.print("Settings: not a float: ");
    Serial.println(entry->key);
    return 0.0f;
  }
  return entry->value.valueFloat;
}

bool Settings::GetBool(SettingId id) const {
  const SettingsEntry *entry = Get(id);
  if (entry == nullptr) {
    return false;
  }
  if (entry->type != SettingType::Bool) {
    Serial.print("Settings: not a bool: ");
    Serial.println(entry->key);
    return false;
  }
  return entry->value.valueBool;
}

const char *Settings::GetString(SettingId id) const {
  const SettingsEntry *entry = Get(id);
  if (entry == nullptr) {
    return "";
  }
  if (entry->type != SettingType::Str) {
    Serial.print("Settings: not a string: ");
    Serial.println(entry->key);
    return "";
  }
  return entry->value.valueString;
}

bool Settings::Set(SettingId id, int32_t value) {
  SettingsEntry *entry = Get(id);
  if (entry == nullptr) {
    return false;
  }
  if (entry->type != SettingType::Int) {
    Serial.print("Settings: not an int: ");
    Serial.println(entry->key);
    return false;
  }
  if (value < entry->min.valueInt) {
    value = entry->min.valueInt;
  }
  if (value > entry->max.valueInt) {
    value = entry->max.valueInt;
  }
  entry->value.valueInt = value;
  Persist(*entry);
  return true;
}

bool Settings::Set(SettingId id, float value) {
  SettingsEntry *entry = Get(id);
  if (entry == nullptr) {
    return false;
  }
  if (entry->type != SettingType::Float) {
    Serial.print("Settings: not a float: ");
    Serial.println(entry->key);
    return false;
  }
  if (value < entry->min.valueFloat) {
    value = entry->min.valueFloat;
  }
  if (value > entry->max.valueFloat) {
    value = entry->max.valueFloat;
  }
  entry->value.valueFloat = value;
  Persist(*entry);
  return true;
}

bool Settings::Set(SettingId id, bool value) {
  SettingsEntry *entry = Get(id);
  if (entry == nullptr) {
    return false;
  }
  if (entry->type != SettingType::Bool) {
    Serial.print("Settings: not a bool: ");
    Serial.println(entry->key);
    return false;
  }
  entry->value.valueBool = value;
  Persist(*entry);
  return true;
}

bool Settings::Set(SettingId id, const char *value) {
  SettingsEntry *entry = Get(id);
  if (entry == nullptr) {
    return false;
  }
  if (entry->type != SettingType::Str) {
    Serial.print("Settings: not a string: ");
    Serial.println(entry->key);
    return false;
  }
  strlcpy(entry->value.valueString, value, SETTING_STRING_MAX);
  Persist(*entry);
  return true;
}

bool Settings::SetFromString(SettingId id, const char *text) {
  const SettingsEntry *entry = Get(id);
  if (entry == nullptr) {
    return false;
  }
  switch (entry->type) {
    case SettingType::Int:
      return Set(id, (int32_t)strtol(text, nullptr, 10));
    case SettingType::Float:
      return Set(id, (float)strtod(text, nullptr));
    case SettingType::Bool:
      return Set(id, ParseBool(text));
    case SettingType::Str:
      return Set(id, text);
  }
  return false;
}

bool Settings::FindId(const char *key, SettingId &outId) const {
  for (int i = 0; i < SettingId::COUNT; i++) {
    if (strcmp(entries[i].key, key) == 0) {
      outId = (SettingId)i;
      return true;
    }
  }
  Serial.print("Settings: unknown key ");
  Serial.println(key);
  return false;
}

void Settings::ResetToDefaults() {
  for (int i = 0; i < SettingId::COUNT; i++) {
    entries[i].value = entries[i].defaultValue;
    Persist(entries[i]);
  }
}

String Settings::ToJson() const {
  String json = "[";
  for (int i = 0; i < SettingId::COUNT; i++) {
    const SettingsEntry &entry = entries[i];
    if (i > 0) {
      json += ',';
    }
    json += "{\"key\":";
    AppendEscaped(json, entry.key);
    json += ",\"label\":";
    AppendEscaped(json, entry.label);
    json += ",\"group\":";
    AppendEscaped(json, entry.group);
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
