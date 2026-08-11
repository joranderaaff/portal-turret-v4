#include "BootState.h"

void BootState::OnActivate() {
  Serial.println("BootState");
  BaseState::OnActivate();
}