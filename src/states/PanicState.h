#pragma once

#include "RoutineState.h"
#include "StateId.h"

class PanicState : public RoutineState {
 public:
  int runCoroutine() override;

 protected:
  void OnRoutineDone() override;
};
