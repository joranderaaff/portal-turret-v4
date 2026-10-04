#include "PanicState.h"
#include "StateMachine.h"

int PanicState::runCoroutine() {
  COROUTINE_BEGIN();
  // TODO: panic behaviour
  COROUTINE_END();
}

void PanicState::OnRoutineDone() {
  stateMachine->GoToState(StateId::Idle);
}
