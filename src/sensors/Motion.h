#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL343.h>

class Motion
{
public:
    void Initialize();
    void Upate(ulong deltaTime);

private:
    Adafruit_ADXL343 accel = Adafruit_ADXL343(12345);
}