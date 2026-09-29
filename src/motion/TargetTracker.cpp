#include "motion/TargetTracker.h"

TargetTracker::TargetTracker(Settings &settingsIn, Radar &radarIn) : settings(settingsIn), radar(radarIn) {
}

void TargetTracker::Initialize() {
  for (uint8_t i = 0; i < TRACK_COUNT; i++) {
    previousPositions[i] = TargetInfo{0, 0};
  }
  distanceTreshold = settings.GetInt(SettingId::MovementTresshold);
}

void TargetTracker::Update(ulong deltaTime) {
  targetsMovedThisFrame = 0;
  uint8_t targetCount = radar.GetTargetCount();
  for (uint8_t i = 0; i < targetCount; i++) {
    previousPositions[i].movedThisFrame = false;
    RadarTarget target = radar.GetTarget(i);
    if (target.available) {
      int8_t diffX = target.x - previousPositions[i].x;
      int8_t diffY = target.y - previousPositions[i].y;
      int8_t distanceSqrd = diffX * diffX + diffY * diffY;
      if (distanceSqrd > distanceTreshold * distanceTreshold) {
        previousPositions[i].x = target.x;
        previousPositions[i].y = target.y;
        previousPositions[i].movedThisFrame = true;
        targetsMovedThisFrame++;
      } else {
        previousPositions[i].movedThisFrame = false;
      }
    }
  }
}

uint8_t TargetTracker::GetTargetsMovedThisFrame() {
  return targetsMovedThisFrame;
}