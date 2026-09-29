#include "BootState.h"
#include <LittleFS.h>

void BootState::OnActivate() {
  BaseState::OnActivate();

  if (!LittleFS.begin()) {
    Serial.println("Littlefs error. Formatting...");
    LittleFS.format();
    LittleFS.begin();
  }
}

void BootState::Update(ulong deltaTime) {}