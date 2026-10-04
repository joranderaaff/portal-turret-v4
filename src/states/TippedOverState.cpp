#include "TippedOverState.h"
#include "StateMachine.h"

int TippedOverState::runCoroutine() {
  COROUTINE_BEGIN();
  // TODO: tipped over behaviour
  COROUTINE_END();
}

void TippedOverState::OnRoutineDone() {
  stateMachine->GoToState(StateId::Idle);
}
