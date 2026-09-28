#include "FiringState.h"
#include "StateMachine.h"

const float pi = 3.1415;

int FiringState::runCoroutine() {
  COROUTINE_BEGIN();
  turret->audio.QueueLoopedAudioCommand(AudioLoopId::Gun);
  shootingStartTime = millis();
  shootTime = random(1000, 2000);
  while (turret->radar.GetTargetCount() > 0 && millis() < shootingStartTime + shootTime) {
    RadarTarget radarTarget = turret->radar.GetTarget(0);
    float angle = atan2(radarTarget.y, radarTarget.x) / pi * 180 - 90;
    turret->gantry.SetRotationZ(angle, false);
    COROUTINE_YIELD();
  }

  turret->audio.QueueLoopedAudioCommand(AudioLoopId::None);
  COROUTINE_AWAIT(!turret->audio.IsPlaying());
  COROUTINE_END();
}

void FiringState::OnRoutineDone() {
  stateMachine->GoToState(StateId::SearchState);
}
