#pragma once

#include "BaseState.h"
#include "states/DiagnoseCommand.h"

// Nothing automatic happens here, the turret only does what the diagnose page asks for.
// There is no way back: a power cycle resets the turret.
class ManualState : public BaseState {
public:
  void OnActivate() override;
  void Update(ulong deltaTime) override;
  void Execute(const DiagnoseCommand &command);

private:
  void StartLedTest();

  bool ledTestRunning = false;
  int ledTestStep = 0;
  ulong ledTestStepTime = 0;
};
