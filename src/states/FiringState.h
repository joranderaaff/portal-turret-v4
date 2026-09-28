#pragma once

#include "RoutineState.h"
#include "StateId.h"

class FiringState : public RoutineState {
 public:
  int runCoroutine() override;

 protected:
  void OnRoutineDone() override;

 private:
  ulong shootTime = 0;
  ulong shootingStartTime = 0;
};
