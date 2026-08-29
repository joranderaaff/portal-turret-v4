#include "ActivateState.h"

void ActivateRoutine::Initialize(Turret& _turret) { turret = &_turret; }

int ActivateRoutine::runCoroutine() {
  COROUTINE_BEGIN();
  turret->gantry.OpenWings();
  COROUTINE_AWAIT(turret->gantry.GetWingLeft().IsOpen() &&
                  turret->gantry.GetWingRight().IsOpen());
  turret->gantry.GetWingLeft().GetGun().Extend();
  turret->gantry.GetWingRight().GetGun().Extend();
  COROUTINE_DELAY(500);
  COROUTINE_END();
}

void ActivateState::Initialize(StateMachine* stateMachine, Turret& turret) {
  BaseState::Initialize(stateMachine, turret);
  activateRoutine.Initialize(turret);
}

void ActivateState::OnActivate() {
  BaseState::OnActivate();
  activateRoutine.reset();
}

void ActivateState::Update(ulong deltaTime) {
  activateRoutine.runCoroutine();
  if (activateRoutine.isDone()) {
    stateMachine->GoToState(StateId::Idle);
  }
}