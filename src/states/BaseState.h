#include "Arduino.h"

class BaseState {
    public:
        void OnActivate();
        void OnDeactivate();
        void Update(ulong deltaTime);
};