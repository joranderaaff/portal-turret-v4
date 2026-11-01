#include "RemoteState.h"

namespace {
constexpr ulong FIRE_KEEP_ALIVE_TIMEOUT_MS = 1000;
constexpr float PARTY_BPM = 128.0f;
constexpr float PARTY_BEAT_MS = 60000.0f / PARTY_BPM;
constexpr float PARTY_RAINBOW_MS = 4000.0f;
constexpr float PARTY_BASE_BRIGHTNESS = 0.3f;
constexpr float PARTY_PULSE_BRIGHTNESS = 0.7f;
} // namespace

void RemoteState::OnActivate() {
  BaseState::OnActivate();
  firing = false;
  partyMode = false;
  turret->gantry.Halt();
  turret->audio.QueueLoopedAudioCommand(AudioLoopId::None);
  turret->logger.Warn("Remote mode, power cycle to reset.");
}

void RemoteState::Update(ulong deltaTime) {
  UpdatePartyMode(deltaTime);
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
  case RemoteCommandType::PartyMode:
    partyMode = true;
    partyTime = 0;
    break;
  }
}

void RemoteState::UpdatePartyMode(ulong deltaTime) {
  if (!partyMode) {
    return;
  }
  partyTime += deltaTime;

  const uint8_t hue = static_cast<uint8_t>(fmodf(partyTime / PARTY_RAINBOW_MS, 1.0f) * 255.0f);
  const float beatPhase = fmodf(partyTime / PARTY_BEAT_MS, 1.0f);
  const float decay = (1.0f - beatPhase) * (1.0f - beatPhase);
  const float brightness = PARTY_BASE_BRIGHTNESS + (PARTY_PULSE_BRIGHTNESS - PARTY_BASE_BRIGHTNESS) * decay;

  const CRGB color = CHSV(hue, 255, static_cast<uint8_t>(brightness * 255.0f));
  turret->light.SetLeftGunLight(color);
  turret->light.SetRightGunLight(color);
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
