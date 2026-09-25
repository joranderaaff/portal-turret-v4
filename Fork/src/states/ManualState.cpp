#include "ManualState.h"

void ManualState::OnActivate() { BaseState::OnActivate(); }

void ManualState::Update(ulong deltaTime) {
  // while (Serial.available() > 0) {
  //   char c = Serial.read();

  //   if (c == '\n') {
  //     buffer[index] = '\0';

  //     if (buffer[0] == 'X' && buffer[1] == ':') {
  //       int value = atoi(&buffer[2]);
  //       turret->gantry.SetRotationX(value);
  //     }

  //     index = 0;
  //   } else if (index < BUFFER_SIZE - 1) {
  //     buffer[index++] = c;
  //   }
  // }
}