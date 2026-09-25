#include "Settings.h"

namespace {
const char *NVS_NAMESPACE = "turret";
const size_t NVS_KEY_MAX = 15;
} // namespace

SettingValue::SettingValue(const char *v) { strlcpy(valueString, v, SETTING_STRING_MAX); }

SettingsEntry::SettingsEntry(const char *key, const char *label, const char *group, SettingType type, SettingValue defaultValue, SettingValue min, SettingValue max) : key(key), label(label), type(type), value(defaultValue), defaultValue(defaultValue), min(min), max(max) {}

SettingsEntry::SettingsEntry(const char *key, const char *label, const char *group, SettingType type, SettingValue defaultValue) : key(key), label(label), type(type), value(defaultValue), defaultValue(defaultValue), min((int32_t)0), max((int32_t)0) {}

// The rows must stay in the same order as SettingId.
Settings::Settings()
    : entries{
          {"AngleOffsetX", "Angle offset X", "Motion", SettingType::Int, (int32_t)0, (int32_t)-90, (int32_t)90},
          {"AngleOffsetZ", "Angle offset Z", "Motion", SettingType::Int, (int32_t)0, (int32_t)-90, (int32_t)90},
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
      prefs.getString(entry.key, entry.value.valueString, SETTING_STRING_MAX);
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