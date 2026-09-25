#pragma once

#include "BaseState.h"

class IdleState : public BaseState {
public:
  void OnActivate() override;
  void Update(ulong deltaTime) override;
};