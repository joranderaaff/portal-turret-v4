#pragma once

#include <stdint.h>

// A remote control action requested from the web UI. Queued by the web server (it runs on another task)
// and handed to StateMachine::RequestRemote by the main loop, which puts the turret in the remote state.
enum class RemoteCommandType : uint8_t {
  Aim,        // x = angle for the X axis, z = angle for the Z axis, in degrees
  OpenWings,
  CloseWings,
  StartFiring, // also acts as a keep-alive, see RemoteState
  StopFiring,
  PartyMode
};

struct RemoteCommand {
  RemoteCommandType type;
  float x;
  float z;
};
