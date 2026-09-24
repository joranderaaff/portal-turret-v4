#pragma once

#include "Arduino.h"
#include "Turret.h"
#include "states/ActivateState.h"
#include "states/BaseState.h"
#include "states/BootState.h"
#include "states/DisengageState.h"
#include "states/FiringState.h"
#include "states/IdleState.h"
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
  IdleState idleState;
  ActivateState activateState;
  FiringState firingState;
  DisengageState disengageState;
  ManualState manualState;
};