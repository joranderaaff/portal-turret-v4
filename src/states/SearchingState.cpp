#include "states/SearchingState.h"
#include "StateMachine.h"
#include "sensors/Radar.h"

void SearchState::OnActivate() {
  Serial.println("SearchState");
  RoutineState::OnActivate();
}

int SearchState::runCoroutine() {
  COROUTINE_BEGIN();
  turret->audio.QueueAudioCommand(AudioId::Searching);
  COROUTINE_AWAIT(turret->audio.IsPlaying());
  COROUTINE_AWAIT(!turret->audio.IsPlaying());

  searchStartTime = millis();
  timeAtChangeRotation = millis() + 500;

  while (true) {
    ulong curMillis = millis();
    ulong timeSearching = curMillis - searchStartTime;

    if (curMillis > timeAtChangeRotation) {
      turret->gantry.SetRotationX(random(-30, 30), false);
      turret->gantry.SetRotationZ(random(-30, 30), false);
      timeAtChangeRotation = curMillis + random(500, 1000);
    }

    if (timeSearching > 1000 && turret->radar.GetTargetCount() > 0) {
      turret->audio.QueueAudioCommand(AudioId::Alarm);
      COROUTINE_AWAIT(turret->audio.IsPlaying());
      COROUTINE_AWAIT(!turret->audio.IsPlaying());
      stateMachine->GoToState(StateId::FiringState);
    }

    if (timeSearching > 5000) {
      turret->audio.QueueAudioCommand(AudioId::Retire);
      COROUTINE_AWAIT(turret->audio.IsPlaying());
      COROUTINE_AWAIT(!turret->audio.IsPlaying());
      stateMachine->GoToState(StateId::Idle);
    }

    COROUTINE_YIELD();
  }

  COROUTINE_END();
}