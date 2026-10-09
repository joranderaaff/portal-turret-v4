#include <Arduino.h>

class AudioLoop {
public:
  AudioLoop(const uint8_t *samplesIn, const int loopPoints[], int loopPointCount, int sampleCount);
  void Read(uint8_t *buffer, int len);
  void Begin();
  void Stop();
  bool IsPlaying();

private:
  const uint8_t *samples;
  const int *loopPoints;
  int currentSegmentIndex = 0;
  int loopSegmentCount = 0;
  int sampleReadIndex = 0;
  int loopCounter = 0;
  int loopStartSample = 0;
  int loopEndSample = 0;
  int totalSampleCount = 0;
  bool isPlaying = false;
  bool isLooping = false;
  bool isStopping = false;
};