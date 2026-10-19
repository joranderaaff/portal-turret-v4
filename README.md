# Portal Turret v4

A Portal-style turret powered by an ESP32-S3 (Lolin S3 Mini). It detects people with a mmWave radar, opens its wings, extends its guns and tracks targets, complete with sound and lights.

![Portal Turret](images/photo.jpg)

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

### X axis

<img src="images/x_axis_001.png" alt="X axis 1" width="500">

<img src="images/x_axis_002.png" alt="X axis 2" width="500">

<img src="images/x_axis_003.png" alt="X axis 3" width="500">

<img src="images/x_axis_004.png" alt="X axis 4" width="500">

### Z axis

<img src="images/z_axis_001.png" alt="Z axis 1" width="500">

<img src="images/z_axis_002.png" alt="Z axis 2" width="500">

<img src="images/z_axis_003.png" alt="Z axis 3" width="500">

<img src="images/z_axis_004.png" alt="Z axis 4" width="500">

### Bottom

<img src="images/bottom_001.png" alt="Bottom 1" width="500">

<img src="images/bottom_002.png" alt="Bottom 2" width="500">

### Eye

<img src="images/eye_001.png" alt="Eye 1" width="500">

### Gantry

<img src="images/gantry_001.png" alt="Gantry 1" width="500">

<img src="images/gantry_002.png" alt="Gantry 2" width="500">

<img src="images/gantry_003.png" alt="Gantry 3" width="500">

### Guns

<img src="images/gun_001.png" alt="Gun 1" width="500">

<img src="images/gun_002.png" alt="Gun 2" width="500">

<img src="images/gun_003.png" alt="Gun 3" width="500">

<img src="images/gun_004.png" alt="Gun 4" width="500">

<img src="images/gun_005.png" alt="Gun 5" width="500">

<img src="images/gun_006.png" alt="Gun 6" width="500">

<img src="images/gun_007.png" alt="Gun 7" width="500">

<img src="images/gun_008.png" alt="Gun 8" width="500">

<img src="images/gun_009.png" alt="Gun 9" width="500">

### Wings

<img src="images/wing_001.png" alt="Wing 1" width="500">

<img src="images/wing_002.png" alt="Wing 2" width="500">

<img src="images/wing_003.png" alt="Wing 3" width="500">

<img src="images/wing_004.png" alt="Wing 4" width="500">

### Leg

<img src="images/leg_001.png" alt="Leg 1" width="500">

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