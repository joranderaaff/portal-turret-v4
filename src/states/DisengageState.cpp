#include "DisengageState.h"
#include "StateMachine.h"

int DisengageState::runCoroutine() {
  COROUTINE_BEGIN();
  turret->audio.QueueAudioCommand(AudioId::Retire);
  COROUTINE_AWAIT(turret->audio.IsPlaying());
  COROUTINE_AWAIT(!turret->audio.IsPlaying());
  turret->gantry.GetWingLeft().GetGun().Retract();
  turret->gantry.GetWingRight().GetGun().Retract();
  COROUTINE_DELAY(500);
  turret->gantry.CloseWings();
  COROUTINE_AWAIT(!turret->gantry.GetWingLeft().IsClosing() &&
                  !turret->gantry.GetWingRight().IsClosing());
  COROUTINE_DELAY(500);
  COROUTINE_END();
}

void DisengageState::OnRoutineDone() {
  stateMachine->GoToState(StateId::Idle);
}
