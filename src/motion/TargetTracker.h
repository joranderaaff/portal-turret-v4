#include "sensors/Radar.h"
#include "settings/Settings.h"
#include <Arduino.h>

struct TargetInfo {
  int16_t x;
  int16_t y;
  bool movedThisFrame;
};

class TargetTracker {
public:
  TargetTracker(Settings &settings, Radar &radar);
  void Initialize();
  uint8_t GetTargetsMovedThisFrame();

private:
  void Update(ulong deltaTime);
  TargetInfo previousPositions[TRACK_COUNT];
  Settings &settings;
  Radar &radar;
  int distanceTreshold = 0;
  uint8_t targetsMovedThisFrame = 0;
};