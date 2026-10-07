#include "sensors/ImpulseDetection.h"

#include <cmath>
#include <limits>

namespace {

// Windows whose standard deviation (after removing the offset) is below this are
// treated as flat and never match. Stops normalising from blowing up sensor noise.
constexpr float MIN_AMPLITUDE = 0.2f;

// Mean squared difference between the normalised window and template at or below
// which we call it a match. 0 is a perfect match. Tune this against the logs.
constexpr float MATCH_THRESHOLD = 0.1f;

// Removes the mean and scales to a standard deviation of 1, so offset (tilt, gravity)
// and amplitude no longer matter, only the shape. Returns the original standard
// deviation.
float Normalise(float *values, int count) {
  float mean = 0.0f;
  for (int i = 0; i < count; i++) {
    mean += values[i];
  }
  mean /= count;

  float variance = 0.0f;
  for (int i = 0; i < count; i++) {
    variance += (values[i] - mean) * (values[i] - mean);
  }
  float deviation = sqrtf(variance / count);

  for (int i = 0; i < count; i++) {
    values[i] -= mean;
    if (deviation > 1e-6f) {
      values[i] /= deviation;
    }
  }
  return deviation;
}

} // namespace

ImpulseDetection::ImpulseDetection() : impulse{8.846f, 8.846f, 8.846f, 8.846f, 8.846f, 8.846f, 8.846f, 8.846f, 8.846f, 8.846f, 8.846f, 8.810f, 8.843f, 8.830f, 8.863f, 8.938f, 8.969f, 9.209f, 9.805f, 10.02f, 10.11f, 9.808f, 9.143f, 8.624f, 8.226f, 8.049f, 7.985f, 8.048f, 8.108f, 8.236f, 8.428f, 8.502f} {
  for (int i = 0; i < sampleCount; i++) {
    samples[i] = 0.0f;
  }
}

bool ImpulseDetection::AddSample(float z) {
  samples[currentIndex++] = z;
  currentIndex = currentIndex % sampleCount;

  float bestScore = std::numeric_limits<float>::infinity();
  int bestWindowSize = 0;
  for (int windowSize = minWindowSize; windowSize <= maxWindowSize; windowSize++) {
    float window[maxWindowSize];
    float target[maxWindowSize];

    // currentIndex is the next write slot, so the newest sample is just before it and a window of windowSize samples starts windowSize places back.
    int start = (currentIndex - windowSize + sampleCount) % sampleCount;

    for (int i = 0; i < windowSize; i++) {
      window[i] = samples[(start + i) % sampleCount];

      // Map window position i onto the template; first sample -> first value, last sample -> last value.
      float position = (float)i * (impulseLength - 1) / (float)(windowSize - 1);
      int sampleIndexA = (int)floorf(position);
      int sampleIndexB = min(sampleIndexA + 1, impulseLength - 1);
      float blend = position - sampleIndexA;
      target[i] = Lerp(impulse[sampleIndexA], impulse[sampleIndexB], blend);
    }

    if (Normalise(window, windowSize) < MIN_AMPLITUDE) {
      continue;
    }
    Normalise(target, windowSize);

    float score = 0;
    for (int i = 0; i < windowSize; i++) {
      float diff = target[i] - window[i];
      score += diff * diff;
    }

    // Average, so long windows aren't penalised for having more terms.
    score /= windowSize;

    if (score < bestScore) {
      bestScore = score;
      bestWindowSize = windowSize;
    }
  }

  return bestScore <= MATCH_THRESHOLD;
}
