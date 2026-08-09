#include "states/StateMachine.h"

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
    return &bootState;
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