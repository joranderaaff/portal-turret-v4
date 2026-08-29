#include "states/StateMachine.h"

void StateMachine::Initialize(Turret& turretIn) {
  bootState.Initialize(this, turretIn);
  idleState.Initialize(this, turretIn);
  activateState.Initialize(this, turretIn);
  disengageState.Initialize(this, turretIn);
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

BaseState* StateMachine::GetState(StateId stateId) {
  switch (stateId) {
    case StateId::Booting:
      return &bootState;
      break;
    case StateId::Activate:
      return &activateState;
      break;
    case StateId::Disengage:
      return &disengageState;
      break;
    case StateId::Idle:
      return &idleState;
  }
  return nullptr;
}

void StateMachine::Update(ulong deltaTime) {
  if (currentState) {
    currentState->Update(deltaTime);
  }
}