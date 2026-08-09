#pragma once

#include "Arduino.h"
#include "states/BaseState.h"

enum class StateId {
  Booting,
  Idle,
};

class StateMachine {
public:
  void GoToState(StateId nextStateId);
  void Update(ulong deltaTime);

private:
  BaseState *GetState(StateId nextStateId);
  BaseState *currentState = nullptr;

  BaseState bootState;
  BaseState idleState;
};