#include "motion/IntertiaServo.h"

void InertiaServo::attach(int pin) {
  servo.setPeriodHertz(50);
  servo.attach(pin, 500, 2400);
}

void InertiaServo::write(int angle) {
  write(angle, false);
}

void InertiaServo::write(int angle, bool instant) {
  targetAngle = angle;
  if (instant) {
    currentAngle = angle;
    velocity = 0.0f;
    servo.write(currentAngle);
  }
}

void InertiaServo::Update(ulong dt) {
  if (dt <= 0) {
    return;
  }

  const float deltaTime = dt / 1000.0f;
  const float distance = targetAngle - currentAngle;
  const float distanceAbs = abs(distance);

  // Target reached
  if (distanceAbs < 0.0001f && abs(velocity) < 0.0001f) {
    currentAngle = targetAngle;
    velocity = 0;
    return;
  }

  // Direction from current position to target
  const int direction = Sign(distance);

  // If we're moving away from the target, brake first.
  const bool movingWrongWay = velocity != 0.0f && Sign(velocity) != direction;

  if (movingWrongWay) {
    velocity = moveVelocityTowardZero(velocity, deceleration, deltaTime);
  }

  // We're either stationary or moving toward the target.
  // Calculate the velocity profile from the current state.
  else {
    const float currentSpeed = abs(velocity);

    /*
     * Calculate the peak velocity that can be reached
     * while still being able to decelerate to zero
     * exactly at the target.
     *
     * distance =
     *   acceleration distance
     * + deceleration distance
     *
     * distance =
     *   (vPeak² - vCurrent²) / (2a)
     *   + vPeak² / (2d)
     *
     * Therefore:
     * vPeak² =
     *   (2 * distance + vCurrent² / acceleration)
     *   / (1 / acceleration + 1 / deceleration)
     */

    const float peakVelocitySquared = (2.0f * distanceAbs + (currentSpeed * currentSpeed) / acceleration) / ((1.0f / acceleration) + (1.0f / deceleration));

    const float peakVelocity = std::sqrt(std::max(0.0f, peakVelocitySquared));

    /*
     * The actual desired peak velocity is limited
     * by maxVelocity.
     *
     * If peakVelocity < maxVelocity:
     *     triangular profile
     *
     * If peakVelocity >= maxVelocity:
     *     trapezoidal profile
     */

    const float desiredPeakVelocity = std::min(peakVelocity, maxVelocity);

    // Decide whether to accelerate or decelerate
    if (currentSpeed < desiredPeakVelocity) {

      velocity += direction * acceleration * deltaTime;

      // Don't accelerate past the profile velocity.
      const float newSpeed = std::abs(velocity);

      if (newSpeed > desiredPeakVelocity) {
        velocity = direction * desiredPeakVelocity;
      }
    } else {
      velocity = moveVelocityTowardZero(velocity, deceleration, deltaTime);
    }
  }

  // Update position using the new velocity
  currentAngle += velocity * deltaTime;

  // Prevent overshooting the target
  if (
      (direction > 0 && currentAngle >= targetAngle) ||
      (direction < 0 && currentAngle <= targetAngle)) {
    currentAngle = targetAngle;
    velocity = 0;
  }

  servo.write(currentAngle);
}

float InertiaServo::moveVelocityTowardZero(float velocity, float deceleration, float dt) {
  const float amount = deceleration * dt;

  if (velocity > 0.0f) {
    return std::max(0.0f, velocity - amount);
  }

  if (velocity < 0) {
    return std::min(0.0f, velocity + amount);
  }

  return 0.0f;
}