#include "ManualState.h"

namespace {
constexpr ulong LED_TEST_STEP_MS = 600;
// The last step stays on: off.
const CRGB LED_TEST_COLORS[] = {CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Black};
constexpr int LED_TEST_STEPS = sizeof(LED_TEST_COLORS) / sizeof(LED_TEST_COLORS[0]);
} // namespace

void ManualState::OnActivate() {
  BaseState::OnActivate();
  ledTestRunning = false;
  turret->gantry.Halt();
  turret->audio.QueueLoopedAudioCommand(AudioLoopId::None);
  turret->logger.Warn("Manual mode, power cycle to reset.");
}

void ManualState::Update(ulong deltaTime) {
  if (!ledTestRunning) {
    return;
  }
  ledTestStepTime += deltaTime;
  if (ledTestStepTime < LED_TEST_STEP_MS) {
    return;
  }
  ledTestStepTime -= LED_TEST_STEP_MS;
  ledTestStep++;
  turret->light.SetOverrideColor(LED_TEST_COLORS[ledTestStep]);
  if (ledTestStep == LED_TEST_STEPS - 1) {
    ledTestRunning = false;
  }
}

void ManualState::Execute(const DiagnoseCommand &command) {
  switch (command.type) {
  case DiagnoseCommandType::SetServo:
    turret->gantry.SetServoRaw(static_cast<GantryServo>(command.id), command.value);
    break;
  case DiagnoseCommandType::LedTest:
    StartLedTest();
    break;
  case DiagnoseCommandType::PlayAudio:
    turret->audio.QueueAudioCommand(static_cast<AudioId>(command.id));
    break;
  }
}

void ManualState::StartLedTest() {
  ledTestRunning = true;
  ledTestStep = 0;
  ledTestStepTime = 0;
  turret->light.SetOverrideColor(LED_TEST_COLORS[0]);
}
