#include "Settings.h"

void Settings::Initialize() {
  entries[SettingId::AngleOffsetX].name = "AngleOffsetX";
  entries[SettingId::AngleOffsetX].type = SettingType::Int;
  entries[SettingId::AngleOffsetX].value = 0;
}