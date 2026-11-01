#pragma once

#include "BaseState.h"
#include "states/RemoteCommand.h"

// The turret does what the remote page asks for and ignores the shake/tilt interrupts.
// There is no way back: a power cycle resets the turret.
class RemoteState : public BaseState {
public:
  void OnActivate() override;
  void Update(ulong deltaTime) override;
  void Execute(const RemoteCommand &command);

private:
  // Aiming and firing are only allowed with both wings open, the guns and axes would hit the closed wings.
  bool WingsOpen();
  void SetFiring(bool firing);

  void UpdatePartyMode(ulong deltaTime);

  bool firing = false;
  bool partyMode = false;
  ulong partyTime = 0;
  // The page repeats StartFiring while the button is held, so a lost release can't leave the guns firing.
  ulong timeSinceFireRequest = 0;
};
