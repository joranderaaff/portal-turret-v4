#include "states/StateMachine.h"

void StateMachine::GoToState(BaseState *nextState) {

  if (currentState) {
    currentState->OnDeactivate();
  }

  currentState = nextState;

  if (currentState) {
    currentState->OnActivate();
  }
}

void StateMachine::Update(ulong deltaTime) {
  if (currentState) {
    currentState->Update(deltaTime);
  }
}