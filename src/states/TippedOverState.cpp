#include "TippedOverState.h"
#include "StateMachine.h"

int TippedOverState::runCoroutine() {
  COROUTINE_BEGIN();
  turret->audio.QueueAudioCommand(AudioId::Tipped);
  COROUTINE_AWAIT(turret->audio.IsPlaying());
  COROUTINE_AWAIT(!turret->audio.IsPlaying());
  turret->gantry.CloseWings();
  COROUTINE_AWAIT(!turret->gantry.GetWingLeft().IsOpen() && !turret->gantry.GetWingRight().IsOpen());

  eyeBrightness= 255;
  while (eyeBrightness > -50) {
    uint8_t noise = inoise8(millis() * 5);
    noise = scale8(noise, 50);
    uint8_t red = constrain(eyeBrightness + noise, 0, 255);
    eyeBrightness -= 1;
    red = dim8_video(red);
    turret->light.SetEyeColor(CRGB(red, 0, 0));
    COROUTINE_DELAY(6); // 255+50 to 0 in about 2 seconds is 6ms per tick
  }

  COROUTINE_AWAIT(turret->gantry.IsAtRest());

  eyeBrightness = 0;
  while (eyeBrightness < 255) {
    turret->light.SetEyeColor(CRGB(dim8_video(eyeBrightness), 0, 0));
    eyeBrightness++;
    COROUTINE_DELAY(8);
  }

  COROUTINE_END();
}

void TippedOverState::OnRoutineDone() {
  stateMachine->GoToState(StateId::Idle);
}
