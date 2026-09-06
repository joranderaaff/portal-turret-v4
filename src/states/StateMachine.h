#pragma once

#include "Arduino.h"
#include "Turret.h"
#include "states/ActivateState.h"
#include "states/BaseState.h"
#include "states/BootState.h"
#include "states/DisengageState.h"
#include "states/ManualState.h"
#include "states/StateId.h"

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
  ActivateState activateState;
  DisengageState disengageState;
  ManualState manualState;
};