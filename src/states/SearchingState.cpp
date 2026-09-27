#include "StateMachine.h"
#include "sensors/Radar.h"
#include "states/SearchState.h"

void SearchState::OnActivate() {
  Serial.println("SearchState");
  timeSearching = 0;
  turret->audio.QueueAudioCommand(AudioId::Searching);
  BaseState::OnActivate();
}

void SearchState::Update(ulong deltaTime) {

  timeSearching += deltaTime;
  if (timeSearching > 3000) {
    stateMachine->GoToState(StateId::Disengage);
    return;
  }

  for (uint8_t i = 0; i < TRACK_COUNT; i++) {
    RadarTarget target = turret->radar.GetTarget(i);
    if (turret->radar.GetTargetCount() > 0 && target.available) {
      stateMachine->GoToState(StateId::FiringState);
      break;
    }
  }
}