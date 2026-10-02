#pragma once

#include <Arduino.h>
#include <Preferences.h>

enum SettingId {
  AngleOffsetX,
  AngleOffsetZ,
  GunMoveTimeout,
  GunHallMin,
  GunHallMax,
  AngleMaxX,
  AngleMaxZ,
  ShootMinTime,
  ShootMaxTime,
  SearchMinTime,
  SearchMaxTime,
  SearchMoveMinTime,
  SearchMoveMaxTime,
  MovementTresshold,
  DelayAfterDisengage,
  ShotDuration,
  ShotBrightness,
  COUNT
};

enum class SettingType { Int,
                         Float,
                         Bool,
                         Str };

constexpr size_t SETTING_STRING_MAX = 32;

union SettingValue {
  int32_t valueInt;
  float valueFloat;
  bool valueBool;
  char valueString[SETTING_STRING_MAX];

  SettingValue(int32_t v) : valueInt(v) {}
  SettingValue(float v) : valueFloat(v) {}
  SettingValue(bool v) : valueBool(v) {}
  SettingValue(const char *v);
};

class SettingsEntry {
public:
  // Numeric settings: min/max bound the value.
  SettingsEntry(const char *key, const char *label, SettingType type, SettingValue defaultValue, SettingValue min, SettingValue max);
  // Bool and string settings: no meaningful range.
  SettingsEntry(const char *key, const char *label, SettingType type, SettingValue defaultValue);

  const char *key;   // NVS + JSON identity, max 15 chars, never rename
  const char *label; // human readable, for the frontend
  SettingType type;
  SettingValue value;
  SettingValue defaultValue;
  SettingValue min;
  SettingValue max;
};

class Settings {
public:
  Settings();
  
  SettingsEntry entries[SettingId::COUNT];

  // Opens NVS and overwrites values that have been stored before.
  void Initialize();

  int32_t GetInt(SettingId id) const;
  float GetFloat(SettingId id) const;
  bool GetBool(SettingId id) const;
  const char *GetString(SettingId id) const;
  
  const SettingsEntry *Get(SettingId id) const;
  SettingsEntry *Get(SettingId id);

  // Clamp to range, store, write to NVS. Returns false on a type mismatch.
  bool Set(SettingId id, int32_t value);
  bool Set(SettingId id, float value);
  bool Set(SettingId id, bool value);
  bool Set(SettingId id, const char *value);

  // For the web layer, which only ever has text.
  bool SetFromString(SettingId id, const char *text);
  bool FindId(const char *key, SettingId &outId) const;

  void ResetToDefaults();

private:
  void Persist(const SettingsEntry &entry);
  void Load();

  Preferences prefs;
  bool prefsReady;
};
