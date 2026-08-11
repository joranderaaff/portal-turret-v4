#pragma once

#include "BaseState.h"

class BootState : public BaseState {
public:
  void OnActivate() override;
};