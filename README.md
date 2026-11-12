# Portal Turret v4

A Portal-style turret powered by an ESP32-S3 (Lolin S3 Mini). It detects people with a mmWave radar, opens its wings, extends its guns and tracks targets, complete with sound and lights.

![Portal Turret](images/photo.jpg)

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/donate/?business=JUNX99Q7QHS9C&no_recurring=0&item_name=Support+my+3D+printing+adventures%21&currency_code=EUR)

## Repository layout

| Folder | Contents |
| --- | --- |
| `src/` | Firmware (PlatformIO, Arduino framework) |
| `data/` | LittleFS image: audio clips and web UI |
| `3d/` | 3D model (`model.step`) |
| `gerber/` | PCB manufacturing files (MainBoard, DaughterBoard) |
| `images/` | Build guide photos |

## Bill of materials

- PLA (Or other material) in white, black and optionally transparent.
- Lolin Wemos S3 Mini
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
- 2 100nF Ceramic Capacitor 
- 1 100uF 25V Elektrolytic Capacitor

- 604ZZ Ball Bearing - 4x12x4mm
- MYT LM6LUU Linear Ball Bearing - 2 pieces (35mm length)
- 4mm steel rod
- 6mm steel rod

- JST PH crimp set OR JST PH headers and pre-crimped JST PH cables
- USB C Breakout Board, 12 * 15 * 1.6mm
- Pin headers
- 2mm wood screws

## Steel Rod Sizes

For the 4mm rods:
- 1x 150mm (enforcing the rear leg)
- 2x 55mm (enforcing the front legs)
- 2x 60mm (wings)
- 1x 43mm (x-axis axle)
- 1x 20mm (z-axis bottom axle)
- 1x 10mm (z-axis top axle)

The bended rods for the legs do not have a very precise length. It's best to bend and cut to length after bending, as this makes the process of getting the correct shape a lot easier.

For the 6mm rods:
- 2x 60mm (wings)

## 3D printing

STEP file contains all 3D printed parts.

[All STL files can be found here.](https://www.printables.com/model/1892132-portal-turret-mk-ii)



## Electronics

Gerber files are included. Order from your favorite PCB manufacturer.

### Pinout

ESP32-S3 GPIO numbers, defined in `src/pins.h`.

| Function | GPIO |
| --- | --- |
| Left / right gun servo | 1 / 2 |
| Left / right wing servo | 4 / 5 |
| Rotate X / Z servo | 6 / 7 |
| Left / right hall sensor (analog) | 8 / 9 |
| Eye LED ring (center) / left gun LEDs / right gun LEDs | 14 / 15 / 16 |
| Radar (ESP RX / ESP TX) | 17 / 18 |
| ADXL345 I2C (SDA / SCL) | 35 / 36 |
| Audio amp I2S (DIN / BCLK / LRCLK) | 10 / 11 / 12 |

### Power

The turret runs on 5 V, from the USB-C board or the power connector. The servos, LED ring, audio amp and radar all share it, so use a supply that can deliver at least 2 A. That figure is a rule of thumb for six SG90 servos plus the LEDs, not a measured value.

## Preperation

The 180 degree servos might not be centered when you purchase them. Make sure you to do this with a servo tester or writing a small Arduino sketch that centers your servos. If you don't have an Arduino board to do this, you could even use the Wemos in a breadboard and use some wires to hook up a servo to calibrate them.

Use the hall sensor to find out what the north and south pole of your magnets are. You'll need to know this to make sure you insert them correctly later in the build.
Again you can use the Wemos to test. Using a breadboard, hook up the sensor to 3.3V, Gnd and one of the analog pins. Read the analog value. When the north pole faces the sensor, the value should increase. If it's the south pole, it should decrease.

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
| Print the flat retina and mold. As PLA gets soft at relatively low temperatures, you can dunk it in boiling water and press it on the mold for the correct shape. Make sure to print the diffuser with very little top/bottom layers and walls, or use transparent filament. Glue in the LED ring and 5mm LED. Bend the 5V and Gnd legs of the LED to the pads of the ring and solder them together. The `D-in` leg should go to the `D-out` pad. Run 3 wires to the `D-in` `5V` and `Gnd` pads of the LED Ring. Keep enough length to run it through the turret. Add a JST connector when you're sure you have enough length. | <img src="images/eye_001.png" alt="Eye 1" height="400"> |

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
| Prepare the LED harness. You can either use JST connectors or solder directly to the board. Make sure you double check the pads of the board and the legs of your LEDs. I've found that different brands can have a different order in the legs. Please note that the `Data In` leg should go to the `Data In` pad, NOT the `Data Out` pad. | <img src="images/led_harness.png" alt="Gun 1" height="400"> |
| Attach the lever to a 180 servo. The lever should be a friction fit. Insert it into the gun casing and run the wire as shown. Note that the LED harness is not shown here, but should already be attached to the board. | <img src="images/gun_001.png" alt="Gun 1" height="400"> |
| There's a hole in the bottom to fasten the servo with a screw. | <img src="images/gun_002.png" alt="Gun 2" height="400"> |
| Insert the barrels into their housing. This should be a friction fit. The housing screws on the part with one screw. | <img src="images/gun_003.png" alt="Gun 2" height="400"> |
| Slide in the bottom LED and run the wires to the side and through the clamp in the front. | <img src="images/gun_004.png" alt="Gun 2" height="400"> |
| Slide in the top LED and run the wires out the top. | <img src="images/gun_005.png" alt="Gun 3" height="400"> |
| Rotate the lever so it points forward. | <img src="images/gun_006.png" alt="Gun 4" height="400"> |
| Carefully slide the barrels over the lever. You might need to bend the housing to get it in place. Note that the pictures shows the wires outside of the clamps, this is a render error. | <img src="images/gun_007.png" alt="Gun 5" height="400"> |
| Slide the barrels back into the housing. They should slide easily. If not, try to move the lever by hand. If the barrels don't move, check the clearance of your parts. | <img src="images/gun_008.png" alt="Gun 6" height="400"> |
| The top can now be screwed on. | <img src="images/gun_009.png" alt="Gun 6" height="400"> |
| The front plate should click in place. | <img src="images/gun_010.png" alt="Gun 6" height="400"> |

### Wings

| Notes | Images |
| --- | --- |
| Add 1mm magnets to the slots of the wing. Mind the polarity! | <img src="images/wing_001.png" alt="Wing 1" height="400"> |
| Add 1mm magnets to the slots of the the wing frame. Mind the polarity! | <img src="images/wing_002.png" alt="Wing 2" height="400"> |
| The gun should slide in the designated slot. | <img src="images/wing_003.png" alt="Wing 3" height="400"> |
| Insert the magnets in the rack gear. They should click in place, but a drop of glue helps. Make sure to mind the polarity!| <img src="images/wing_004.png" alt="Wing 4" height="400"> |
| Attach the rack gear and secure it with a screw. Glue in the 4mm and 6mm steel rods. | <img src="images/wing_005.png" alt="Wing 4" height="400"> |

### PCB, Speaker and Radar

| Notes | Images |
| --- | --- |
| PCB, Speaker and Radar should be mounted in the top. Note that the ADXL and Amplifier are missing in this picture. They should be soldered on the PCB. | <img src="images/top_001.png" height="400"> |
| There are optional panels to cover the electronics. | <img src="images/top_002.png" height="400"> |


### Leg
| Notes | Images |
| --- | --- |
| The legs should be glued together. Use a piece of 4mm rod to strengthen the joint between the "leg" and the "knee" | <img src="images/leg_001.png" alt="Leg 1" height="400"> |

## Firmware

You can build the firmware with <a href="https://code.visualstudio.com/">Visual Studio Code</a> together with the PlatformIO plugin.

Use the following two commands to flash your Wemos:

<img src="images/building.png">

Or use these commands if you prefer working in the terminal.

```bash
pio run                # build
pio run -t upload      # flash firmware
pio run -t uploadfs    # flash LittleFS (audio + web UI)
```

Flash both on a new turret: the firmware first, then the filesystem. Run `uploadfs` again whenever something in `data/` changes.

OTA updates are possible once the turret is on your network (`turret.local`) or in its own access point ("Portal Turret", `192.168.4.1`). While an update runs the turret stops moving and the audio pauses.

### First start and WiFi

1. On the first start, and whenever the turret can't join its saved network, it opens the WiFi network "Portal Turret".
2. Connect to it. The WiFi setup page opens by itself, or browse to `http://192.168.4.1`.
3. Pick your network and enter the password. The turret restarts, joins your network and is reachable at `http://turret.local`.
4. If the saved network is out of reach (for example the router is still booting after a power cut), the "Portal Turret" network returns. The turret tries the saved network again every minute, as long as nobody is connected to its access point, and closes the access point once it is back on your network.

The access point is open by default. To protect it, enter 8 or more characters as "Access Point Password" on the Settings page and restart the turret. The access point keeps that password when you reset the settings to their defaults, so it can't fall back to open by accident.

### Web interface

| Page | What it does |
| --- | --- |
| Home | Live radar, accelerometer graphs and the turret's message log. |
| Control | Start the turret by hand. |
| Remote | Aim with a joystick, open/close the wings, fire, party mode. |
| Settings | All settings, Restart, reset to defaults, enable multitarget on the radar. |
| WiFi | Pick the network the turret joins. |
| Diagnose | Move every servo raw, read the hall sensors, test LEDs and audio. |

Settings take effect as soon as you save them. Only the network settings (WiFi, access point password) need a restart: use the Restart button on the Settings page.

Remote and Diagnose stop all automatic behaviour. Press Resume on either page to hand control back: the turret centers the gantry, closes the wings and starts over. A power cycle does the same.

## Calibration

When connecting to the web portal, there is a `Settings` page. Most of these settings are tweaked already, but you might need a few to get your turret to work perfectly:
You can use the `Diagnose` page to find the exact values you want to use (the hall sensor readings help with Gun Hall Min/Max).

- Angle offset X: Use this to make sure the wings are pointing forward when the turret is idle.
- Angle offset Z: Use this to center the gantry when the turret is idle.
- Gun Hall Min/Max: The treshold of the hall sensor that triggers the Open/Closed state of the wings.

The other parameters are less important, but you can play around with them to suit your needs.