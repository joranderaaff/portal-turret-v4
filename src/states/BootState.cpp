#include "BootState.h"
#include "StateMachine.h"

int BootState::runCoroutine() {
  COROUTINE_BEGIN();
  turret->gantry.GetWingLeft().Close(true);
  turret->gantry.GetWingRight().Close(true);
  COROUTINE_AWAIT(!turret->gantry.GetWingLeft().IsOpen() && !turret->gantry.GetWingRight().IsOpen());
  COROUTINE_DELAY(1000);
  COROUTINE_AWAIT(!turret->settings.GetBool(SettingId::BlockStart));
  COROUTINE_END();
}

bool BootState::CheckInterrupt(StateId &next) {
  return false;
}

void BootState::OnRoutineDone() {
  stateMachine->GoToState(StateId::Idle);
}