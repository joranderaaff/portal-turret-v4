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
#include "states/PanicState.h"
#include "states/TippedOverState.h"
#include "states/SearchingState.h"
#include "states/StateId.h"

class StateMachine {
public:
  void Initialize(Turret &turret);
  void GoToState(StateId nextStateId);
  // Starts the turret (Idle -> Activate), ignoring the BlockStart setting. No-op outside Idle.
  void RequestStart();
  // Runs a diagnose action. Switches to the manual state first, which stops all automatic behaviour.
  void RequestDiagnose(const DiagnoseCommand &command);
  void Update(ulong deltaTime);

private:
  BaseState *GetState(StateId nextStateId);
  BaseState *currentState = nullptr;
  StateId currentStateId = StateId::Booting;
  Turret *turret = nullptr;

  BootState bootState;
  IdleState idleState;
  ActivateState activateState;
  FiringState firingState;
  SearchState searchState;
  DisengageState disengageState;
  ManualState manualState;
  TippedOverState tippedOverState;
  PanicState panicState;
};