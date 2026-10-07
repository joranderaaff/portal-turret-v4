#include "TippedOverState.h"
#include "StateMachine.h"

int TippedOverState::runCoroutine() {
  COROUTINE_BEGIN();
  turret->gantry.CloseWings();
  COROUTINE_AWAIT(!turret->gantry.GetWingLeft().IsOpen() && !turret->gantry.GetWingRight().IsOpen());
  COROUTINE_AWAIT(turret->gantry.IsAtRest());
  COROUTINE_END();
}

void TippedOverState::OnRoutineDone() {
  stateMachine->GoToState(StateId::Idle);
}
