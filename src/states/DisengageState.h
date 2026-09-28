#pragma once

#include "RoutineState.h"
#include "StateId.h"

class DisengageState : public RoutineState {
 public:
  int runCoroutine() override;

 protected:
  void OnRoutineDone() override;
};
