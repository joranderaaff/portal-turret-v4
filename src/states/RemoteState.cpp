#include "RemoteState.h"

namespace {
constexpr ulong FIRE_KEEP_ALIVE_TIMEOUT_MS = 1000;
} // namespace

void RemoteState::OnActivate() {
  BaseState::OnActivate();
  firing = false;
  turret->gantry.Halt();
  turret->audio.QueueLoopedAudioCommand(AudioLoopId::None);
  turret->logger.Warn("Remote mode, power cycle to reset.");
}

void RemoteState::Update(ulong deltaTime) {
  if (!firing) {
    return;
  }
  timeSinceFireRequest += deltaTime;
  if (!WingsOpen() || timeSinceFireRequest > FIRE_KEEP_ALIVE_TIMEOUT_MS) {
    SetFiring(false);
  }
}

void RemoteState::Execute(const RemoteCommand &command) {
  switch (command.type) {
  case RemoteCommandType::Aim:
    if (!WingsOpen()) {
      break;
    }
    turret->gantry.SetRotationX(command.x, false);
    turret->gantry.SetRotationZ(command.z, false);
    break;
  case RemoteCommandType::OpenWings:
    turret->gantry.OpenWings();
    break;
  case RemoteCommandType::CloseWings:
    SetFiring(false);
    turret->gantry.CloseWings();
    break;
  case RemoteCommandType::StartFiring:
    if (!WingsOpen()) {
      break;
    }
    timeSinceFireRequest = 0;
    SetFiring(true);
    break;
  case RemoteCommandType::StopFiring:
    SetFiring(false);
    break;
  }
}

bool RemoteState::WingsOpen() {
  return turret->gantry.GetWingLeft().IsOpen() && turret->gantry.GetWingRight().IsOpen();
}

void RemoteState::SetFiring(bool shouldFire) {
  if (shouldFire == firing) {
    return;
  }
  firing = shouldFire;
  if (firing) {
    turret->gantry.GetWingLeft().GetGun().StartFiring();
    turret->gantry.GetWingRight().GetGun().StartFiring();
  } else {
    turret->gantry.GetWingLeft().GetGun().StopFiring();
    turret->gantry.GetWingRight().GetGun().StopFiring();
  }
}
