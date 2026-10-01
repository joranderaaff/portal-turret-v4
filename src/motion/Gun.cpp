#include "Gun.h"

const int EXTEND_ANGLE = 160;

Gun::Gun(Settings &_settings, int servoPinIn) : settings(_settings) {
  servoPin = servoPinIn;
}

void Gun::Initialize() {
  servo.setPeriodHertz(50); // standard 50 hz servo
  servo.attach(servoPin, 500, 2400);
  shotDuration = settings.GetInt(SettingId::ShotDuration);
  Retract();
}

void Gun::Update(ulong deltaTime) {
  if (firing) {
    firingTime += deltaTime;
    uint phaseOffset = servoPin == PIN_GUN_LEFT ? 0 : shotDuration >> 2;
    uint currentShotPhase = (firingTime + phaseOffset) % shotDuration;
    // Move phase of one gun half the duration
    float p = (float)currentShotPhase / shotDuration;
    float attack = 0.20f;
    // https://graphtoy.com/?f1(x,t)=1-max((x-0.2)/(1-0.2),(x-0.2)*-(1/0.2))&v1=true&f2(x,t)=&v2=true&f3(x,t)=&v3=false&f4(x,t)=&v4=false&f5(x,t)=&v5=false&f6(x,t)=&v6=false&grid=1
    float q = 1.0f - max((p - attack) / (1.0f - attack), (p - attack) * -(1.0f / attack));
    q = Clamp(q, 0.0f, 1.0f);

    float extendAngle = servoPin == PIN_GUN_LEFT ? EXTEND_ANGLE : 180.0f - extendAngle;
    float angle = Lerp(90, extendAngle, q);

    if (servoPin == PIN_GUN_LEFT) {
      Serial.print("firingTime: ");
      Serial.print(firingTime);
      Serial.print(" currentShotPhase: ");
      Serial.print(currentShotPhase);
      Serial.print(" Q: ");
      Serial.print(q);
      Serial.print(" Angle: ");
      Serial.println(angle);
    }

    servo.write(angle);
  }
}

void Gun::Extend() {
  if (servoPin == PIN_GUN_LEFT) {
    servo.write(EXTEND_ANGLE);
  } else {
    servo.write(180 - EXTEND_ANGLE);
  }
}

void Gun::Retract() {
  servo.write(90);
}

void Gun::StartFiring() {
  firingTime = 0;
  firing = true;
}

void Gun::StopFiring() {
  firing = false;
  Retract();
}