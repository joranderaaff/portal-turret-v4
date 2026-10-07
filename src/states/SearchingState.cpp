#include "states/SearchingState.h"
#include "StateMachine.h"
#include "sensors/Radar.h"

void SearchState::OnActivate() {
  RoutineState::OnActivate();
}

int SearchState::runCoroutine() {
  COROUTINE_BEGIN();
  turret->audio.QueueAudioCommand(AudioId::Searching);
  COROUTINE_AWAIT(turret->audio.IsPlaying());
  COROUTINE_AWAIT(!turret->audio.IsPlaying());

  doLoop = true;
  searchStartTime = millis();
  timeAtChangeRotation = millis() + settings->GetInt(SettingId::SearchMoveMinTime);
  movementWasDetected = false;
  while (doLoop) {
    ulong curMillis = millis();
    ulong timeSearching = curMillis - searchStartTime;

    if (curMillis > timeAtChangeRotation) {
      turret->gantry.SetRotationX(random(-30, 30), false);
      turret->gantry.SetRotationZ(random(-30, 30), false);
      timeAtChangeRotation = curMillis + random(settings->GetInt(SettingId::SearchMoveMinTime), settings->GetInt(SettingId::SearchMoveMaxTime));
    }

    if (turret->targetTracker.GetTargetsMovedThisFrame() > 0) {
      movementWasDetected = true;
    }

    //Also wait until the voice line is over before we go into the alarm and re-fire
    if (movementWasDetected && timeSearching > settings->GetInt(SettingId::SearchMinTime) && !turret->audio.IsPlaying()) {
      Serial.print(" RE-TRIGGER: ");
      Serial.print(" timeSearching: ");
      Serial.print(timeSearching);
      Serial.print(" settings->GetInt(SettingId::SearchMinTime): ");
      Serial.print(settings->GetInt(SettingId::SearchMinTime));
      Serial.println("");


      turret->audio.QueueAudioCommand(AudioId::Alarm);
      COROUTINE_AWAIT(turret->audio.IsPlaying());
      COROUTINE_AWAIT(!turret->audio.IsPlaying());
      nextState = StateId::FiringState;
      doLoop = false;
    }
    else if (timeSearching > settings->GetInt(SettingId::SearchMaxTime)) {
      Serial.print(" DISENGAGE: ");
      Serial.print(" timeSearching: ");
      Serial.print(timeSearching);
      Serial.print(" settings->GetInt(SettingId::SearchMaxTime): ");
      Serial.print(settings->GetInt(SettingId::SearchMaxTime));
      Serial.println("");
      nextState = StateId::Disengage;
      doLoop = false;
    }

    COROUTINE_YIELD();
  }

  COROUTINE_END();
}

bool SearchState::CheckInterrupt(StateId& next) {
  return InterruptForOrientation(next);
}

void SearchState::OnRoutineDone() {
  stateMachine->GoToState(nextState);
}