#include "RoutineState.h"
#include "StateMachine.h"

void RoutineState::Initialize(StateMachine *stateMachine, Turret &turret) {
  BaseState::Initialize(stateMachine, turret);
  settings = &turret.settings;
}

void RoutineState::OnActivate() {
  BaseState::OnActivate();
  reset();
}

void RoutineState::Update(ulong deltaTime) {
  runCoroutine();
  if (isDone()) {
    OnRoutineDone();
  }
}

void RoutineState::OnRoutineDone() {}
