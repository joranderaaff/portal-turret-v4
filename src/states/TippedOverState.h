#pragma once

#include "RoutineState.h"
#include "StateId.h"

class TippedOverState : public RoutineState {
 public:
  int runCoroutine() override;

 protected:
  void OnRoutineDone() override;
};
