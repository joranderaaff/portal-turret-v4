#pragma once

#include "RoutineState.h"
#include "StateId.h"

class ActivateState : public RoutineState {
 public:
  int runCoroutine() override;

 protected:
  void OnRoutineDone() override;
};
