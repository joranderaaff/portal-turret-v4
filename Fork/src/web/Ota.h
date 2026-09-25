#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

// Over-the-air firmware upload over the existing web server.
// POST the firmware.bin to /update as multipart form data, e.g.
//   curl -F "firmware=@.pio/build/lolin_s3_mini/firmware.bin" http://192.168.4.1/update
class Ota {
public:
  void Initialize(AsyncWebServer &server);
  void Update(ulong deltaTime);

private:
  ulong rebootTimer = 0;
  bool shouldReboot = false;
};
