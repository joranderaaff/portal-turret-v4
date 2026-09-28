#include "ActivateState.h"
#include "StateMachine.h"

int ActivateState::runCoroutine() {
  COROUTINE_BEGIN();
  turret->audio.QueueAudioCommand(AudioId::Activate);
  COROUTINE_AWAIT(turret->audio.IsPlaying());
  COROUTINE_AWAIT(!turret->audio.IsPlaying());
  turret->gantry.OpenWings();
  COROUTINE_AWAIT(turret->gantry.GetWingLeft().IsOpen() && turret->gantry.GetWingRight().IsOpen());
  turret->gantry.GetWingLeft().GetGun().Extend();
  turret->gantry.GetWingRight().GetGun().Extend();
  COROUTINE_DELAY(500);
  COROUTINE_END();
}

void ActivateState::OnRoutineDone() {
  stateMachine->GoToState(StateId::FiringState);
}