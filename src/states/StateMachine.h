#pragma once

#include "Arduino.h"
#include "Turret.h"
#include "states/BaseState.h"
#include "states/BootState.h"

enum class StateId {
  Booting,
  Idle,
};

class StateMachine {
public:
  void Initialize(Turret &turret);
  void GoToState(StateId nextStateId);
  void Update(ulong deltaTime);

private:
  BaseState *GetState(StateId nextStateId);
  BaseState *currentState = nullptr;

  BootState bootState;
  BaseState idleState;
};