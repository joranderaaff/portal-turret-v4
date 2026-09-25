#pragma once

#include "BaseState.h"

class ManualState : public BaseState {
public:
  void OnActivate() override;
  void Update(ulong deltaTime) override;

private:
  const int BUFFER_SIZE = 32;
  char buffer[32];
  int index = 0;
};