#pragma once

#include <AceRoutine.h>
#include "BaseState.h"
#include "StateId.h"

class SearchingRoutine : public ace_routine::Coroutine {
public:
  void Initialize(StateMachine &stateMachine, Turret &turret);
  int runCoroutine() override;

private:
  Turret *turret;
  StateMachine *stateMachine;
  ulong searchStartTime = 0;
  ulong timeAtChangeRotation = 0;
};

class SearchState : public BaseState {
public:
  void Initialize(StateMachine *stateMachine, Turret &turret) override;
  void OnActivate() override;
  void Update(ulong deltaTime) override;

private:
  ulong timeSearching = 0;
  SearchingRoutine searchingRoutine;
};