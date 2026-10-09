#include "AudioLoop.h"

AudioLoop::AudioLoop(const uint8_t *samplesIn, const int loopPointsIn[], int loopPointCount, int sampleCount) {
  loopPoints = loopPointsIn;
  loopSegmentCount = loopPointCount - 1;
  samples = samplesIn;
  totalSampleCount = sampleCount;
}

void AudioLoop::Begin() {
  isPlaying = true;
  isLooping = false;
  isStopping = false;

  loopStartSample = loopPoints[currentSegmentIndex];
  loopEndSample = loopPoints[currentSegmentIndex + 1];

  loopCounter = 0;
  sampleReadIndex = 0;
}

bool AudioLoop::IsPlaying() {
  return isPlaying;
}

void AudioLoop::Stop() {
  if (isStopping) {
    return;
  }
  isLooping = false;
  isStopping = true;
}

void AudioLoop::Read(uint8_t *buffer, int len) {
  for (int i = 0; i < len; i += 2) {
    if (!isPlaying) {
      buffer[i + 0] = 0;
      buffer[i + 1] = 0;
    } else {
      int byteReadIndex = sampleReadIndex * 2;
      buffer[i + 0] = samples[byteReadIndex + 0];
      buffer[i + 1] = samples[byteReadIndex + 1];

      sampleReadIndex++;

      if (!isLooping && loopCounter == 0 &&
          sampleReadIndex >= loopStartSample) {
        isLooping = true;
      }

      if (isLooping && sampleReadIndex >= loopEndSample) {

        int sectionLength = loopEndSample - loopStartSample;
        sampleReadIndex -= sectionLength;
        int sampleOffset = sampleReadIndex - loopStartSample;

        Serial.print(" sectionLength: ");
        Serial.print(sectionLength);

        Serial.print(" currentSegmentIndex: ");
        Serial.print(currentSegmentIndex);

        currentSegmentIndex = random(0, loopSegmentCount);

        loopStartSample = loopPoints[currentSegmentIndex];
        loopEndSample = loopPoints[currentSegmentIndex + 1];

        loopCounter++;

        Serial.println("");
      }

      if (sampleReadIndex >= totalSampleCount) {
        isPlaying = false;
      }
    }
  }
}