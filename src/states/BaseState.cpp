#include "BaseState.h"
#include "StateMachine.h"

void BaseState::Initialize(StateMachine *stateMachineIn, Turret &turretIn) {
  stateMachine = stateMachineIn;
  turret = &turretIn;
}

void BaseState::OnActivate() {}

void BaseState::OnDeactivate() {}

void BaseState::Update(ulong deltaTime) {}