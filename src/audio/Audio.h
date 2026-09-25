#pragma once

#include "Arduino.h"
#include "AudioLoop.h"
#include "AudioTools.h"
#include "AudioTools/AudioCodecs/CodecMP3Helix.h"
#include "AudioTools/Disk/AudioSourceLittleFS.h"
#include "GunShotAudio.h"
#include "driver/i2s.h"
#include "pins.h"

enum AudioType {
  Activate,
  Searching,
  Pickup,
  Tipped,
  Retire,
};

class Audio {
public:
  Audio();
  void Initialize();
  void Update(ulong deltaTime);
  void PlaySound(AudioType type);
  bool IsPlaying();
  AudioLoop ShootAudio;

private:
  void GetRandomAudio(AudioType type);
  char filename[32];
  int sampleReadIndex = 0;
  int loopCounter = 0;
  bool isLooping = false;
  bool isPlaying = false;
  uint8_t sampleBuffer[4096];
  I2SStream i2s;
  AudioSourceLittleFS source;
  MP3DecoderHelix mp3Decoder;
  EncodedAudioStream decoder;
  StreamCopy copier;
};