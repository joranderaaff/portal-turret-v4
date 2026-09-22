#include "BootState.h"
#include <LittleFS.h>
#include "audio/ESP32Downloader.h"

void BootState::OnActivate() {
  Serial.println("BootState");
  BaseState::OnActivate();

  if (!LittleFS.begin()) {
    Serial.println("Littlefs error. Formatting...");
    LittleFS.format();
    LittleFS.begin();
  }
}

void BootState::Update(ulong deltaTime) {}