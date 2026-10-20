#pragma once

#include "RoutineState.h"
#include "StateId.h"

class BootState : public RoutineState {
public:
  int runCoroutine() override;
  bool CheckInterrupt(StateId& next) override;

protected:
  void OnRoutineDone() override;
};
