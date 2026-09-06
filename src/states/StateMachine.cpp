#include "states/StateMachine.h"

void StateMachine::Initialize(Turret &turretIn) {
  bootState.Initialize(this, turretIn);
  idleState.Initialize(this, turretIn);
  activateState.Initialize(this, turretIn);
  disengageState.Initialize(this, turretIn);
  manualState.Initialize(this, turretIn);
}

void StateMachine::GoToState(StateId nextStateId) {
  if (currentState) {
    currentState->OnDeactivate();
  }

  currentState = GetState(nextStateId);

  if (currentState) {
    currentState->OnActivate();
  }
}

BaseState *StateMachine::GetState(StateId stateId) {
  switch (stateId) {
  case StateId::Booting:
    Serial.println("Bootstate");
    return &bootState;
    break;
  case StateId::Activate:
    Serial.println("ActivateState");
    return &activateState;
    break;
  case StateId::Disengage:
    Serial.println("DisengageState");
    return &disengageState;
    break;
  case StateId::Idle:
    Serial.println("IdleState");
    return &idleState;
  case StateId::Manual:
    Serial.println("ManualState");
    return &manualState;
  }
  return nullptr;
}

void StateMachine::Update(ulong deltaTime) {
  if (currentState) {
    currentState->Update(deltaTime);
  }
}