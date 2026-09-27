#pragma once

#include "BaseState.h"

class SearchState : public BaseState {
public:
  void OnActivate() override;
  void Update(ulong deltaTime) override;

private:
  ulong timeSearching = 0;
};