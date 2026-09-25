#pragma once

#include "Arduino.h"
#include "AudioLoop.h"
#include "AudioTools.h"
#include "AudioTools/AudioCodecs/CodecMP3Helix.h"
#include "AudioTools/Disk/AudioSourceLittleFS.h"
#include "GunShotAudio.h"
#include "driver/i2s.h"
#include "pins.h"

class Audio {
public:
  Audio();
  void Initialize();
  void Update(ulong deltaTime);

  AudioLoop ShootAudio;

private:
  int sampleReadIndex = 0;
  int loopCounter = 0;
  bool isLooping = false;
  uint8_t sampleBuffer[4096];
  I2SStream i2s;
  AudioSourceLittleFS source;
  MP3DecoderHelix decoder;
  AudioPlayer player;
};