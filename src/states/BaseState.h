#pragma once

#include "Arduino.h"
#include "Turret.h"
#include "states/StateId.h"

class StateMachine;  // forward declaration

class BaseState {
 public:
  virtual void Initialize(StateMachine* stateMachine, Turret& turret);
  virtual void OnActivate();
  virtual void OnDeactivate();
  virtual void Update(ulong deltaTime);

  virtual bool CheckInterrupt(StateId& next) { return false; }

 protected:
  bool InterruptForOrientation(StateId& next);

  StateMachine* stateMachine = nullptr;
  Turret* turret = nullptr;
};