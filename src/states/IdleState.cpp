#include "states/IdleState.h"
#include "StateMachine.h"
#include "sensors/Radar.h"

void IdleState::OnActivate() {
  BaseState::OnActivate();
  turret->logger.Info("Standby.");
}

void IdleState::Update(ulong deltaTime) {
  if (turret->targetTracker.GetTargetsMovedThisFrame() > 0) {
    stateMachine->GoToState(StateId::Activate);
  }
}

bool IdleState::CheckInterrupt(StateId& next) {
  return InterruptForOrientation(next);
}