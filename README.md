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

- Lolin Wemos S3
- LD2450 Radar Sensor
- OH49E Hall Effect Sensor - SS49E compatible
- ADXL345 Digital 3-axis Accelerometer Module
- MAX98357 Audio amp
- SN74AHCT125N 4-channel Level Converter
- Small rocker switch

- SK6812 Digital 5050 RGB LED Ring
- 4 SG90 180 servo
- 2 SG90 360 servo
- 5 WS2812 5mm LED

- 604ZZ Ball Bearing - 4x12x4mm
- MYT LM6LUU Linear Ball Bearing - 2 pieces (35mm length)
- 4mm steel rod
- 6mm steel rod

- JST PH crimp set OR JST PH headers and pre-crimped JST PH cables
- Pin headers
- 2mm wood screws

## 3D printing

STEP file contains all 3D printed parts. Separate parts in correct print orientation might come later.

## Electronics

Gerber files are included. Order from your favorite PCB manufacturer.

## Assembly guide

Here's raw screenshots. Explicit information comes later.

### X axis

| Notes | Images |
| --- | --- |
| Screw the two halves together. | <img src="images/x_axis_001.png" alt="X axis 2" style="background-color: #2e3440; object-fit: contain;" width="400" height="400"> |
| Insert the LM6LUU and 604ZZ bearings. Screw the gears on the two 360 servos. | <img src="images/x_axis_002.png" alt="X axis 2" style="background-color: #2e3440; object-fit: contain;" width="400" height="400"> |
| Solder the wires to the hall sensors. Use shrink tubing to protect the leads. Insert the hall sensors into the slots. The beveled face is the front and should point towards the space where the rack gears will slide. | <img src="images/x_axis_003.png" alt="X axis 3" style="background-color: #2e3440; object-fit: contain;" width="400" height="400"> | 
| Tuck the wires underneath the little lip. This should help with keeping the hall sensors in place. If they don't want to stay seated, a drop of superglue should help. | <img src="images/x_axis_004.png" alt="X axis 4" style="background-color: #2e3440; object-fit: contain;" width="400" height="400"> | 

### Z axis
| Notes | Images |
| --- | --- |
| Screw on the X Axis gear on a 180 servo. Slide it in place and use the channels to screw it down. | <img src="images/z_axis_001.png" alt="Z axis 1" style="background-color: #2e3440; object-fit: contain;" width="400" height="400"> |
| Attach one side. | <img src="images/z_axis_002.png" alt="Z axis 2" style="background-color: #2e3440; object-fit: contain;" width="400" height="400"> |
| Slide a 4mm rod through the X axis. Add a 4mm shim on both sides. There is a slot in the frame where the rod should fit. | <img src="images/z_axis_003.png" alt="Z axis 3" style="background-color: #2e3440; object-fit: contain;" width="400" height="400"> |
| Scew on the bottom and the other side. | <img src="images/z_axis_004.png" alt="Z axis 4" style="background-color: #2e3440; object-fit: contain;" width="400" height="400"> |
| Use a ziptie to manage all cables. It should hold the following wires: 2 Hall sensors, 2 Wing servos, X Axis servo. | <img src="images/z_axis_005.png" alt="Z axis 4" style="background-color: #2e3440; object-fit: contain;" width="400" height="400"> |

### Bottom
| Notes | Images |
| --- | --- |
| Screw the Z Axis gear on a 180 servo. Attach the servo to the Servo Mount. The USB-C board should be glued into place. If you can't get a solid connection with glue, a tiny screw with some heatshrink could be screwed through the D- slot. Make sure the screw does not short D- to ground, hence the heatshrink. | <img src="images/bottom_001.png" alt="Bottom 1" style="background-color: #2e3440; object-fit: contain;" width="400" height="400"> |
| The main board power wire should have the black wire soldered to the GND pad of the USB board. The red wire should slide to the bottom. Another red wire should be soldered to the 5V (Vcc) pad of the USB board. Slide those through the channels. Attach the two power wires by appropriate connectors or soldering to the switch and slide it in the bottom. Route the servo wire up through the same channels. | <img src="images/bottom_003.png" alt="Bottom 2" style="background-color: #2e3440; object-fit: contain;" width="400" height="400"> |
| Slide the switch in the slot on the bottom. | <img src="images/bottom_002.png" alt="Bottom 2" style="background-color: #2e3440; object-fit: contain;" width="400" height="400"> |
| The servo mount is attached to the bottom with 4 more screws. | <img src="images/bottom_004.png" alt="Bottom 2" style="background-color: #2e3440; object-fit: contain;" width="400" height="400"> |

### Eye
| Notes | Images |
| --- | --- |
|  | <img src="images/eye_001.png" alt="Eye 1" style="background-color: #2e3440; object-fit: contain;" width="400" height="400"> |

### Gantry

| Notes | Images |
| --- | --- |
|  | <img src="images/gantry_001.png" alt="Gantry 1" style="background-color: #2e3440; object-fit: contain;" width="400" height="400"> |
|  | <img src="images/gantry_002.png" alt="Gantry 2" style="background-color: #2e3440; object-fit: contain;" width="400" height="400"> |
|  | <img src="images/gantry_003.png" alt="Gantry 3" style="background-color: #2e3440; object-fit: contain;" width="400" height="400"> |

### Guns

| Notes | Images |
| --- | --- |
|  | <img src="images/gun_001.png" alt="Gun 1" style="background-color: #2e3440; object-fit: contain;" width="400" height="400"> |
|  | <img src="images/gun_002.png" alt="Gun 2" style="background-color: #2e3440; object-fit: contain;" width="400" height="400"> |
|  | <img src="images/gun_003.png" alt="Gun 3" style="background-color: #2e3440; object-fit: contain;" width="400" height="400"> |
|  | <img src="images/gun_004.png" alt="Gun 4" style="background-color: #2e3440; object-fit: contain;" width="400" height="400"> |
|  | <img src="images/gun_005.png" alt="Gun 5" style="background-color: #2e3440; object-fit: contain;" width="400" height="400"> |
|  | <img src="images/gun_006.png" alt="Gun 6" style="background-color: #2e3440; object-fit: contain;" width="400" height="400"> |
|  | <img src="images/gun_007.png" alt="Gun 7" style="background-color: #2e3440; object-fit: contain;" width="400" height="400"> |
|  | <img src="images/gun_008.png" alt="Gun 8" style="background-color: #2e3440; object-fit: contain;" width="400" height="400"> |
|  | <img src="images/gun_009.png" alt="Gun 9" style="background-color: #2e3440; object-fit: contain;" width="400" height="400"> |

### Wings

| Notes | Images |
| --- | --- |
|  | <img src="images/wing_001.png" alt="Wing 1" style="background-color: #2e3440; object-fit: contain;" width="400" height="400"> |
|  | <img src="images/wing_002.png" alt="Wing 2" style="background-color: #2e3440; object-fit: contain;" width="400" height="400"> |
|  | <img src="images/wing_003.png" alt="Wing 3" style="background-color: #2e3440; object-fit: contain;" width="400" height="400"> |
|  | <img src="images/wing_004.png" alt="Wing 4" style="background-color: #2e3440; object-fit: contain;" width="400" height="400"> |

### Leg
| Notes | Images |
| --- | --- |
|  | <img src="images/leg_001.png" alt="Leg 1" style="background-color: #2e3440; object-fit: contain;" width="400" height="400"> |

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