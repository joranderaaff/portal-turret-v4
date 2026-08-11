#pragma once

#include "audio/Audio.h"
#include "light/Light.h"
#include "motion/Gantry.h"
#include "sensors/Motion.h"
#include "sensors/Radar.h"

struct Turret {
  Gantry &gantry;
  Motion &motion;
  Radar &radar;
  Audio &audio;
  Light &light;
};