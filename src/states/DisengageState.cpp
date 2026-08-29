#include "DisengageState.h"

void DisengageRoutine::Initialize(Turret& _turret) { turret = &_turret; }

int DisengageRoutine::runCoroutine() {
  COROUTINE_BEGIN();
  turret->gantry.GetWingLeft().GetGun().Extend();
  turret->gantry.GetWingRight().GetGun().Extend();
  COROUTINE_DELAY(500);
  turret->gantry.CloseWings();
  COROUTINE_AWAIT(!turret->gantry.GetWingLeft().IsOpen() &&
                  !turret->gantry.GetWingRight().IsOpen());
  COROUTINE_END();
}

void DisengageState::Initialize(StateMachine* stateMachine, Turret& turret) {
  BaseState::Initialize(stateMachine, turret);
  disengageRoutine.Initialize(turret);
}

void DisengageState::OnActivate() {
  BaseState::OnActivate();
  disengageRoutine.reset();
}

void DisengageState::Update(ulong deltaTime) {
  disengageRoutine.runCoroutine();
  if (disengageRoutine.isDone()) {
    stateMachine->GoToState(StateId::Idle);
  }
}