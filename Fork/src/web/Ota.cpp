#include "Ota.h"
#include <Update.h>

namespace {
const ulong RebootDelay = 500;
}

void Ota::Initialize(AsyncWebServer &server) {
  server.on(
      "/update", HTTP_POST,
      [this](AsyncWebServerRequest *request) {
        bool failed = ::Update.hasError();
        AsyncWebServerResponse *response = request->beginResponse(
            failed ? 500 : 200, "text/plain",
            failed ? ::Update.errorString() : "Update OK, rebooting");
        response->addHeader("Connection", "close");
        request->send(response);

        if (!failed) {
          shouldReboot = true;
          rebootTimer = RebootDelay;
        }
      },
      [](AsyncWebServerRequest *request, String filename, size_t index,
         uint8_t *data, size_t len, bool final) {
        if (index == 0) {
          Serial.printf("OTA start: %s\n", filename.c_str());
          if (!::Update.begin(UPDATE_SIZE_UNKNOWN)) {
            ::Update.printError(Serial);
            return;
          }
        }

        if (::Update.hasError()) {
          return;
        }

        if (::Update.write(data, len) != len) {
          ::Update.printError(Serial);
          return;
        }

        if (final) {
          if (::Update.end(true)) {
            Serial.printf("OTA done: %u bytes\n", index + len);
          } else {
            ::Update.printError(Serial);
          }
        }
      });
}

void Ota::Update(ulong deltaTime) {
  if (!shouldReboot) {
    return;
  }

  if (rebootTimer > deltaTime) {
    rebootTimer -= deltaTime;
    return;
  }

  Serial.println("Rebooting into new firmware");
  Serial.flush();
  ESP.restart();
}
