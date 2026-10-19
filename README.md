# Portal Turret v4

A Portal-style turret powered by an ESP32-S3 (Lolin S3 Mini). It detects people with a mmWave radar, opens its wings, extends its guns and tracks targets, complete with sound and lights.

_Introduction and finished-turret photo go here._

## Repository layout

| Folder | Contents |
| --- | --- |
| `src/` | Firmware (PlatformIO, Arduino framework) |
| `data/` | LittleFS image: audio clips and web UI |
| `3d/` | 3D model (`model.step`) |
| `gerber/` | PCB manufacturing files (MainBoard, DaughterBoard) |
| `images/` | Build guide photos |

## Bill of materials

Lolin Wemos S3
LD2450 Radar Sensor
OH49E Hall Effect Sensor - SS49E compatible
ADXL345 Digital 3-axis Accelerometer Module
MAX98357 Audio amp
SN74AHCT125N 4-channel Level Converter

SK6812 Digital 5050 RGB LED Ring
4 SG90 180 servo
2 SG90 360 servo
5 WS2812 5mm LED

604ZZ Ball Bearing - 4x12x4mm
MYT LM6LUU Linear Ball Bearing - 2 pieces (35mm length)
4mm steel rod
6mm steel rod

JST PH crimp set OR JST PH headers and pre-crimped JST PH cables
Pin headers
2mm wood screws

## 3D printing

STEP file contains all 3D printed parts. Separate parts in correct print orientation might come later.

## Electronics

Gerber files are included. Order from your favorite PCB manufacturer.

## Assembly guide

Here's raw screenshots. Explicit information comes later.

### Bottom

![Bottom 1](images/bottom_001.png)

![Bottom 2](images/bottom_002.png)

### Leg

![Leg 1](images/leg_001.png)

### Gantry

![Gantry 1](images/gantry_001.png)

![Gantry 2](images/gantry_002.png)

![Gantry 3](images/gantry_003.png)

### X axis

![X axis 1](images/x_axis_001.png)

![X axis 2](images/x_axis_002.png)

![X axis 3](images/x_axis_003.png)

![X axis 4](images/x_axis_004.png)

### Z axis

![Z axis 1](images/z_axis_001.png)

![Z axis 2](images/z_axis_002.png)

![Z axis 3](images/z_axis_003.png)

![Z axis 4](images/z_axis_004.png)

### Eye

![Eye 1](images/eye_001.png)

### Wings

![Wing 1](images/wing_001.png)

![Wing 2](images/wing_002.png)

![Wing 3](images/wing_003.png)

![Wing 4](images/wing_004.png)

### Guns

![Gun 1](images/gun_001.png)

![Gun 2](images/gun_002.png)

![Gun 3](images/gun_003.png)

![Gun 4](images/gun_004.png)

![Gun 5](images/gun_005.png)

![Gun 6](images/gun_006.png)

![Gun 7](images/gun_007.png)

![Gun 8](images/gun_008.png)

![Gun 9](images/gun_009.png)

## Firmware

Built with [PlatformIO](https://platformio.org/) (env `lolin_s3_mini`).

```bash
pio run                # build
pio run -t upload      # flash firmware
pio run -t uploadfs    # flash LittleFS (audio + web UI)
```

OTA updates are possible once the turret is on your network (`turret.local`) or in its own access point ("Portal Turret", `192.168.4.1`).

More info coming soon

## Calibration

More info coming soon