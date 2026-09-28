#pragma once

#include "RoutineState.h"
#include "StateId.h"

class SearchState : public RoutineState {
public:
  void OnActivate() override;
  int runCoroutine() override;

private:
  ulong searchStartTime = 0;
  ulong timeAtChangeRotation = 0;
};
