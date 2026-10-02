#pragma once

#include "WiFi.h"
#include "settings/Settings.h"
#include <ESPAsyncWebServer.h>
#include <Turret.h>

enum WebSocketMessage : uint8_t {
  MESSAGE_RADAR = 0x01,
  MESSAGE_MOTION = 0x02,
};

// Radar message layout (little endian):
//   u8  MESSAGE_RADAR
//   u8  target count (n)
//   n x { u8 id, u8 flags (bit0 available, bit1 isMoving),
//         i16 x, i16 y, i16 previousX, i16 previousY, i16 speed, u16 resolution }
// Positions are in mm.
constexpr size_t RADAR_TARGET_BYTES = 14;
constexpr size_t RADAR_MESSAGE_BYTES = 2 + TRACK_COUNT * RADAR_TARGET_BYTES;

// Motion message layout (little endian):
//   u8  MESSAGE_MOTION
//   3 x f32 acceleration x, y, z in m/s^2
constexpr size_t MOTION_MESSAGE_BYTES = 1 + 3 * sizeof(float);

class TurretWebServer {
public:
  TurretWebServer();
  void Initialize(Turret &turret, Settings &settings);
  void Update(ulong deltaTime);
  AsyncWebServer webServer;

private:
  void SendRadar();
  void SendMotion();

  AsyncWebSocket socket;
  Settings *settings;
  Turret *turret;
  ulong timeSinceRadarSend = 0;
};
