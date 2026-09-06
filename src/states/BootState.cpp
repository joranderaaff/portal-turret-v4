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

  // uint8_t sounds_number[9] = {8, 5, 6, 8, 10, 7, 10, 6, 13};
  // char filename[256];
  // for (int folder = 1; folder <= sizeof(sounds_number); folder++) {
  //   snprintf(filename, 255, "/%02i", folder);
  //   LittleFS.mkdir(filename);
  //   for (int filenum = 1; filenum <= sounds_number[folder - 1]; filenum++) {
  //     snprintf(filename, 255, "/%02i/%03i.mp3", folder, filenum);
  //     if (!LittleFS.exists(filename)) {
  //       downloadFile(filename, filename);
  //     }
  //   }
  // }
}

void BootState::Update(ulong deltaTime) {}