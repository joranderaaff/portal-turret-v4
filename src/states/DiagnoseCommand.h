#pragma once

#include <stdint.h>

// A diagnose action requested from the web UI. The web server queues these (it runs on another task)
// and the main loop hands them to StateMachine::RequestDiagnose, which puts the turret in manual mode.
enum class DiagnoseCommandType : uint8_t {
  SetServo, // id = GantryServo, value = raw angle 0..180
  LedTest,  // cycles all LEDs through red, green, blue, off
  PlayAudio // id = AudioId
};

struct DiagnoseCommand {
  DiagnoseCommandType type;
  int32_t id;
  int32_t value;
};
