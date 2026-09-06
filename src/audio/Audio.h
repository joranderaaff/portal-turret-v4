#pragma once

#include "Arduino.h"
#include "AudioTools.h"
#include "AudioTools/AudioCodecs/CodecMP3Helix.h"
#include "AudioTools/Disk/AudioSourceLittleFS.h"
#include "driver/i2s.h"
#include "pins.h"
#include <math.h>

class Audio {
public:
  Audio();
  void Initialize();
  void Update(ulong deltaTime);
  void PlaySound();
  void LoopSound();

private:
  I2SStream i2s;
  AudioSourceLittleFS source;
  MP3DecoderHelix decoder;
  AudioPlayer player;
};