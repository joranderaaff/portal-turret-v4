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
| Screw the two halves together. | <img src="images/x_axis_001.png" alt="X axis 2" height="400"> |
| Insert the LM6LUU and 604ZZ bearings. Screw the gears on the two 360 servos. | <img src="images/x_axis_002.png" alt="X axis 2" height="400"> |
| Solder the wires to the hall sensors. Use shrink tubing to protect the leads. Insert the hall sensors into the slots. The beveled face is the front and should point towards the space where the rack gears will slide. | <img src="images/x_axis_003.png" alt="X axis 3" height="400"> | 
| Tuck the wires underneath the little lip. This should help with keeping the hall sensors in place. If they don't want to stay seated, a drop of superglue should help. | <img src="images/x_axis_004.png" alt="X axis 4" height="400"> | 

### Z axis
| Notes | Images |
| --- | --- |
| Screw on the X Axis gear on a 180 servo. Slide it in place and use the channels to screw it down. | <img src="images/z_axis_001.png" alt="Z axis 1" height="400"> |
| Attach one side. | <img src="images/z_axis_002.png" alt="Z axis 2" height="400"> |
| Slide a 4mm rod through the X axis. Add a 4mm shim on both sides. There is a slot in the frame where the rod should fit. | <img src="images/z_axis_003.png" alt="Z axis 3" height="400"> |
| Scew on the bottom and the other side. | <img src="images/z_axis_004.png" alt="Z axis 4" height="400"> |
| Use a ziptie to manage all cables. It should hold the following wires: 2 Hall sensors, 2 Wing servos, X Axis servo. | <img src="images/z_axis_005.png" alt="Z axis 4" height="400"> |

### Bottom
| Notes | Images |
| --- | --- |
| Slide a 180 servo in the mount and screw it secure | <img src="images/bottom_001.png" alt="Bottom 2" height="400"> |
| Screw on the Z Axis gear. | <img src="images/bottom_002.png" alt="Bottom 2" height="400"> |
| The USB-C board should be glued into place. If you can't get a solid connection with glue, a tiny screw with some heatshrink could be screwed through the D- slot. Make sure the screw does not short D- to ground, hence the heatshrink tubing. Run the wires through the bottom. Make sure to leave enough to route the wires and attach the JST connector. | <img src="images/bottom_003.png" alt="Bottom 2" height="400"> |
| Route the two 5V wires to the bottom. One should come from the USB board, the other should go to your power connector. | <img src="images/bottom_004.png" alt="Bottom 2" height="400"> |
| The servo wires should also be run through the same channel to go up in the back. | <img src="images/bottom_005.png" alt="Bottom 2" height="400"> |
| Run both the servo and power wires through the gantry. Use the little brace to keep them out of the way from the gantry.  | <img src="images/bottom_006.png" alt="Bottom 2" height="400"> |

### Eye
| Notes | Images |
| --- | --- |
| Print the flat retina and mold. As PLA gets soft at relatively low temperatures, you can dunk it in boiling water and press it on the mold for the correct shape. Make sure to print the diffuser with very little top/bottom layers and walls, or use transparent filament. Glue in the LED ring and 5mm LED.| <img src="images/eye_001.png" alt="Eye 1" height="400"> |

### Gantry

| Notes | Images |
| --- | --- |
| The gantry consists of 5 different pieces. This is done so you can print with little to no support material. The eye should click in place and stay there without any glue. | <img src="images/gantry_001.png" alt="Gantry 1" height="400"> |
| The whole gantry should screw together and give enough rigidity for the build. | <img src="images/gantry_002.png" alt="Gantry 2" height="400"> |
| Slide in the gantry and bottom. Make sure to not rotate your servo when assembling, otherwise you'll risk a very big offset in angle. You can calibrate small offsets in the settings. | <img src="images/gantry_003.png" alt="Gantry 3" height="400"> |
| Screw in the last screws on the front and back of the gantry to finish the motion system. | <img src="images/gantry_004.png" alt="Gantry 3" height="400"> |

### Guns

| Notes | Images |
| --- | --- |
| Attach the lever to a 180 servo. The lever should be a friction fit. | <img src="images/gun_001.png" alt="Gun 1" height="400"> |
| The barrels should slide in place with a bit of friction. Use one screw to assemble the two parts. | <img src="images/gun_002.png" alt="Gun 2" height="400"> |
| Move the lever so it points forwards. | <img src="images/gun_003.png" alt="Gun 3" height="400"> |
| Slide the barrel mount over the lever an drotate it back | <img src="images/gun_004.png" alt="Gun 4" height="400"> |
| Carefully slide the assembly in the housing.  | <img src="images/gun_005.png" alt="Gun 5" height="400"> |
| Press the barrles back in the housing. | <img src="images/gun_006.png" alt="Gun 6" height="400"> |
| The top can now be screwed on. | <img src="images/gun_007.png" alt="Gun 6" height="400"> |
| The front plate should click in place. | <img src="images/gun_008.png" alt="Gun 6" height="400"> |

### Wings

| Notes | Images |
| --- | --- |
| Add 1mm magnets to the slots of the wing. Mind the polarity! | <img src="images/wing_001.png" alt="Wing 1" height="400"> |
| Add 1mm magnets to the slots of the the wing frame. Mind the polarity! | <img src="images/wing_002.png" alt="Wing 2" height="400"> |
| The gun should slide in the designated slot. | <img src="images/wing_003.png" alt="Wing 3" height="400"> |
| Attach the rack gear and secure it with a screw. Glue in the 4mm and 6mm steel rods. | <img src="images/wing_004.png" alt="Wing 4" height="400"> |

### Leg
| Notes | Images |
| --- | --- |
| The legs should be glued together. Use a piece of 4mm rod to strengthen the joint between the "leg" and the "knee" | <img src="images/leg_001.png" alt="Leg 1" height="400"> |

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