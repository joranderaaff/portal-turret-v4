#include <Arduino.h>
#include "sensors/Radar.h"
#include "settings/Settings.h"

class TargetTracker {
public:
  TargetTracker(Settings &settings, Radar &radar);

private:
  void Update(ulong deltaTime);
  Settings &settings;
  Radar &radar;
};