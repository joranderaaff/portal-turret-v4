#include "AudioLoop.h"

AudioLoop::AudioLoop(const uint8_t *samplesIn, int loopStartSampleIn, int loopEndSampleIn, int sampleCount) {
  loopStartSample = loopStartSampleIn;
  loopEndSample = loopEndSampleIn;
  samples = samplesIn;
  totalSampleCount = sampleCount;
}

void AudioLoop::Begin() {
  isPlaying = true;
  isLooping = false;
  isStopping = false;
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
        sampleReadIndex -= loopEndSample - loopStartSample;
        loopCounter++;
      }

      if (sampleReadIndex >= totalSampleCount) {
        isPlaying = false;
      }
    }
  }
}