#include <Arduino.h>

class AudioLoop {
 public:
  AudioLoop(const uint8_t* samplesIn, int loopStartSampleIn, int loopEndSampleIn);
  void Read(uint8_t* buffer, uint8_t len);
  void Begin();
  void Stop();

 private:
  const uint8_t* samples;
  int sampleReadIndex = 0;
  int loopCounter = 0;
  int loopStartSample;
  int loopEndSample;
  bool isPlaying = false;
  bool isLooping = false;
};