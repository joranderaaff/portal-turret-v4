#pragma once

#include "Arduino.h"
#include "AudioLoop.h"
#include "AudioTools.h"
#include "AudioTools/AudioCodecs/CodecMP3Helix.h"
#include "AudioTools/Disk/AudioSourceLittleFS.h"
#include "GunShotAudio.h"
#include "pins.h"

enum class AudioType {
  None,
  Activate,
  Searching,
  Pickup,
  Tipped,
  Retire,
};

enum class GunAudioRequestType {
  None,
  Start,
  Stop
};

class Audio {
public:
  Audio();
  void Initialize();
  void RequestSound(AudioType type);
  void RequestGunSoundChange(GunAudioRequestType requestType);
  bool IsPlaying();
  AudioLoop ShootAudio;
  
  private:
  std::atomic<AudioType> requestedAudio{AudioType::None};
  std::atomic<GunAudioRequestType> playGunAudio{GunAudioRequestType::None};
  
  volatile bool isPlaying = false;
  
  static void AudioTask(void *arg);
  void Update(ulong deltaTime);
  void GetRandomAudio(AudioType type);
  char filename[32];
  uint8_t sampleBuffer[4096];
  I2SStream i2s;
  AudioSourceLittleFS source;
  MP3DecoderHelix mp3Decoder;
  EncodedAudioStream decoder;
  StreamCopy copier;
};