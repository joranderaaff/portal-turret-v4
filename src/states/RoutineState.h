#pragma once

#include <AceRoutine.h>

#include "BaseState.h"
#include "settings/Settings.h"

class RoutineState : public BaseState, public ace_routine::Coroutine {
 public:
  void Initialize(StateMachine* stateMachine, Turret& turret) override;
  void OnActivate() override;
  void Update(ulong deltaTime) override;

 protected:
  virtual void OnRoutineDone();
  Settings* settings = nullptr;
};
