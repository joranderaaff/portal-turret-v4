#include "Arduino.h"

#define TRACK_COUNT 3

struct RadarTarget {
  uint8_t id;
  int16_t x;
  int16_t y;
  int16_t previousX;
  int16_t previousY;
  int16_t speed;
  uint16_t resolution;
  bool available;
};

class Radar {
public:
  void Initialize();
  void Update(ulong deltaTime);

private:
  RadarTarget radarTargets[TRACK_COUNT];
  bool readingData = false;
  bool readingAck = false;

  uint8_t radarTargetCount;
  uint8_t previousRadarTargetCount;

  uint8_t writeIndex = 0;

  uint8_t headerIndex = 0;
  uint8_t footerIndex = 0;

  uint8_t messageHeader[4] = {0XAA, 0xFF, 0x03, 0x00};
  uint8_t messageFooter[2] = {0x55, 0xCC};

  uint8_t ackHeaderIndex = 0;
  uint8_t ackFooterIndex = 0;
  uint8_t ackHeader[4] = {0XFD, 0xFC, 0xFB, 0xFA};
  uint8_t ackFooter[4] = {0x04, 0x03, 0x02, 0x01};

  uint8_t messageBuffer[256];

  ulong lastSensorUpdateTime;
};