#pragma once

#include "RoutineState.h"
#include "StateId.h"

class PanicState : public RoutineState {
public:
  int runCoroutine() override;
  bool CheckInterrupt(StateId& next) override;

protected:
  void OnRoutineDone() override;

private:
  int panicMoveCount = 0;
  int panicDelay = 0;
  bool isPanicking;
};
