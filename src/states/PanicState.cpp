#include "PanicState.h"
#include "StateMachine.h"

int PanicState::runCoroutine() {
  COROUTINE_BEGIN();
  isPanicking = true;

  turret->audio.QueueLoopedAudioCommand(AudioLoopId::None);

  COROUTINE_AWAIT(!turret->audio.IsPlaying());

  while (isPanicking) {
    turret->audio.QueueAudioCommand(AudioId::Pickup);
    for (panicMoveCount = 0; isPanicking && panicMoveCount < 10; panicMoveCount++) {
      turret->gantry.SetRotationX(random(-30, 30), false);
      turret->gantry.SetRotationZ(random(-30, 30), false);
      panicDelay = random(settings->GetInt(SettingId::PanicMoveMinTime), settings->GetInt(SettingId::PanicMoveMaxTime));
      
      COROUTINE_DELAY(panicDelay);

      if (turret->gantry.IsAtRest()) {
        isPanicking = false;
      }
    }
  }
  COROUTINE_END();
}

bool PanicState::CheckInterrupt(StateId &next) {
  if (turret->gantry.IsTippedOver()) {
    next = StateId::TippedOver;
    return true;
  }
  return false;
}

void PanicState::OnRoutineDone() {
  stateMachine->GoToState(StateId::Idle);
}