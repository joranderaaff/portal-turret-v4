#include "BaseState.h"
#include "StateMachine.h"

void BaseState::Initialize(StateMachine *stateMachineIn) {
    stateMachine = stateMachineIn;
}

void BaseState::OnActivate() {
    
}

void BaseState::OnDeactivate() {

}

void BaseState::Update(ulong deltaTime) {
    
}