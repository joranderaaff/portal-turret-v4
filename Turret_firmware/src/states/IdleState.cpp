#include "states/IdleState.h"
#include "StateMachine.h"
#include "sensors/Radar.h"

void IdleState::OnActivate() {
  Serial.println("IdleState");
  BaseState::OnActivate();
}

void IdleState::Update(ulong deltaTime) {
  for (uint8_t i = 0; i < TRACK_COUNT; i++) {
    RadarTarget target = turret->radar.GetTarget(i);

    Serial.print("turret->radar.GetTargetCount(): ");
    Serial.println(turret->radar.GetTargetCount());

    if (turret->radar.GetTargetCount() > 0 && target.available) {
      stateMachine->GoToState(StateId::Activate);
      break;
    }
  }
}