#include "BootState.h"

void BootState::OnActivate() {
  Serial.println("BootState");
  BaseState::OnActivate();
  turret->gantry.OpenWings();
}

void BootState::Update(ulong deltaTime) {}