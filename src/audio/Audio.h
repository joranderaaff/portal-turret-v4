#pragma once

#include "Arduino.h"
#include "AudioLoop.h"
#include "LayeredAudio.h"
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

enum class LayeredAudioId {
  Trigger
};

class Audio {
public:
  Audio(Settings &settings);
  void QueueAudioCommand(AudioId nextAudio);
  void QueueLoopedAudioCommand(AudioLoopId nextAudio);
  void QueueLayeredAudioCommand(LayeredAudioId nextAudio);
  void Initialize();
  bool IsPlaying();

private:
  Settings &settings;
  AudioLoop *currentLoopedAudio = nullptr;
  
  LayeredAudio ShootAudio;

  QueueHandle_t audioCommandQueue = NULL;
  QueueHandle_t loopedAudioCommandQueue = NULL;
  QueueHandle_t layeredAudioCommandQueue = NULL;

  volatile bool isPlaying = false;
  // Shot path bookkeeping: estimated bytes waiting in the I2S DMA queue, and a mixed chunk the
  // queue hasn't fully accepted yet
  float queuedBytes = 0;
  ulong lastQueueUpdateUs = 0;
  int pendingBytes = 0;
  // Starts false so the first SetWriteBlocking(true) in Initialize applies it to the driver
  bool writeBlocking = false;
  void SetWriteBlocking(bool blocking);

  static void AudioTask(void *arg);
  void Update(ulong deltaTime);
  
  void GetRandomSoundByType(AudioId type);
  AudioLoop *GetLoopendSoundByType(AudioLoopId type);
  LayeredAudio *GetLayeredSoundByType(LayeredAudioId type);

  char filename[32];
  uint8_t sampleBuffer[4096];
  I2SStream i2s;
  MP3DecoderHelix mp3Decoder;
  VolumeStream volumeStream{i2s};
  EncodedAudioStream decoder{&volumeStream, &mp3Decoder};
  AudioSourceLittleFS source;
  StreamCopy copier;
};