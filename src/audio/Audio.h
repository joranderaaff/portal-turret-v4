#pragma once

#include "Arduino.h"
#include "AudioLoop.h"
#include "AudioTools.h"
#include "AudioTools/AudioCodecs/CodecMP3Helix.h"
#include "AudioTools/Disk/AudioSourceLittleFS.h"
#include "GunShotAudio.h"
#include "pins.h"
#include "settings/Settings.h"

enum class AudioId {
  None,
  Activate,
  Searching,
  Pickup,
  Tipped,
  Retire,
  Alarm,
};

enum class AudioLoopId {
  None,
  Gun
};

class Audio {
public:
  Audio(Settings &settings);
  void QueueAudioCommand(AudioId nextAudio);
  void QueueLoopedAudioCommand(AudioLoopId nextAudio);
  void Initialize();
  bool IsPlaying();

private:
  Settings &settings;
  AudioLoop ShootAudio;
  AudioLoop *currentLoopedAudio = nullptr;

  QueueHandle_t audioCommandQueue = NULL;
  QueueHandle_t loopedAudioCommandQueue = NULL;

  volatile bool isPlaying = false;

  static void AudioTask(void *arg);
  void Update(ulong deltaTime);
  void GetRandomSoundByType(AudioId type);
  AudioLoop *GetLoopendSoundByType(AudioLoopId type);
  char filename[32];
  uint8_t sampleBuffer[4096];
  I2SStream i2s;
  MP3DecoderHelix mp3Decoder;
  VolumeStream volumeStream{i2s};
  EncodedAudioStream decoder{&volumeStream, &mp3Decoder};
  AudioSourceLittleFS source;
  StreamCopy copier;
};