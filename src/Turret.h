#pragma once

#include "audio/Audio.h"
#include "light/Light.h"
#include "logging/Logger.h"
#include "motion/Gantry.h"
#include "motion/TargetTracker.h"
#include "sensors/ADXL.h"
#include "sensors/Radar.h"
#include "settings/Settings.h"

struct Turret {
  Gantry &gantry;
  ADXL &motion;
  Radar &radar;
  Audio &audio;
  Light &light;
  Settings &settings;
  TargetTracker &targetTracker;
  Logger &logger;
};