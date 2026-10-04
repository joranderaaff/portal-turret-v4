#include "BaseState.h"
#include "StateMachine.h"

void BaseState::Initialize(StateMachine *stateMachineIn, Turret &turretIn) {
  stateMachine = stateMachineIn;
  turret = &turretIn;
}

void BaseState::OnActivate() {}

void BaseState::OnDeactivate() {}

void BaseState::Update(ulong deltaTime) {}

bool BaseState::InterruptForOrientation(StateId &next) {
  if (turret->gantry.IsTippedOver()) {
    next = StateId::TippedOver;
    return true;
  }
  if (turret->gantry.IsPickedUp()) {
    next = StateId::Panic;
    return true;
  }
  return false;
}