#pragma once

#include "Arduino.h"

class StateMachine;  // forward declaration

class BaseState {
public:
  void Initialize(StateMachine *stateMachine);
  void OnActivate();
  void OnDeactivate();
  void Update(ulong deltaTime);

private:
  StateMachine *stateMachine;
};