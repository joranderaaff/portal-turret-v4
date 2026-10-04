#pragma once

#include "RoutineState.h"
#include "StateId.h"

class SearchState : public RoutineState {
public:
  void OnActivate() override;
  int runCoroutine() override;
  bool CheckInterrupt(StateId& next) override;

 protected:
  void OnRoutineDone() override;

private:
  bool movementWasDetected = false;
  bool doLoop = true;
  StateId nextState;
  ulong searchStartTime = 0;
  ulong timeAtChangeRotation = 0;
};
