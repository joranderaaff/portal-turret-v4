#include "states/IdleState.h"
#include "StateMachine.h"
#include "sensors/Radar.h"

void IdleState::OnActivate() {
  Serial.println("IdleState");
  BaseState::OnActivate();
}

void IdleState::Update(ulong deltaTime) {
  for (uint8_t i = 0; i < TRACK_COUNT; i++) {
    RadarTarget target = turret->radar.GetTarget(i);
    if (turret->targetTracker.GetTargetsMovedThisFrame() > 0) {
      stateMachine->GoToState(StateId::Activate);
      break;
    }
  }
}