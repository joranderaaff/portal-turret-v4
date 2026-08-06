#include "Arduino.h"
#include "states/BaseState.h"

class StateMachine {
public:
  void GoToState(BaseState *nextState);
  void Update(ulong deltaTime);

private:
  BaseState *currentState = nullptr;
};