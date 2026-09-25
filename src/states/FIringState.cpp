#include "FiringState.h"
#include "StateMachine.h"

void FiringRoutine::Initialize(Turret &_turret) { turret = &_turret; }

const float pi = 3.1415; 

int FiringRoutine::runCoroutine() {
  COROUTINE_BEGIN();
  turret->audio.ShootAudio.Begin();
  shootingStartTime = millis();
  
  while(turret->radar.GetTargetCount() > 0 && millis() < shootingStartTime + 10000) {
    RadarTarget radarTarget = turret->radar.GetTarget(0);
    float angle = atan2(radarTarget.y, radarTarget.x) / pi * 180 - 90;
    turret->gantry.SetRotationZ(angle, false);
    COROUTINE_YIELD();
  }

  // while (millis() < shootingStartTime + 3000) {
  //   COROUTINE_YIELD();
  // }
  turret->audio.ShootAudio.Stop();
  COROUTINE_AWAIT(!turret->audio.ShootAudio.IsPlaying());
  COROUTINE_END();
}

void FiringState::Initialize(StateMachine *stateMachine, Turret &turret) {
  BaseState::Initialize(stateMachine, turret);
  firingRoutine.Initialize(turret);
}

void FiringState::OnActivate() {
  BaseState::OnActivate();
  firingRoutine.reset();
}

void FiringState::Update(ulong deltaTime) {
  firingRoutine.runCoroutine();
  if (firingRoutine.isDone()) {
    stateMachine->GoToState(StateId::Disengage);
  }
}