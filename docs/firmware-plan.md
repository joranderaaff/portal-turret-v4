# Plan d'adaptation du firmware à la carte Turret2

Rédigé le 24.09.2026. Le suivi de l'exécution (ce qui est fait, en cours, les erreurs) est dans [firmware-journal.md](firmware-journal.md) : **lire le journal avant de reprendre le travail**.

> Dates au format JJ.MM.AAAA. Les références `fichier:ligne` pointent sur le code tel qu'il était au commit `020a839` (firmware upstream de joranderaaff, avant toute modification).

---

## 0. Contexte et sources

- **Firmware** : celui de `src/` (upstream [joranderaaff/portal-turret-v4](https://github.com/joranderaaff/portal-turret-v4)), écrit pour un Wemos LOLIN S3 mini câblé à la main (« V4 »).
- **Nouvelle carte** : Turret2, carte 4 couches à ESP32-S3-MINI-1-N8 conçue par lo26lo. Sa documentation (README Turret2, `design-plan.md`, `status-and-history.md`) et le projet KiCad **ne sont pas dans ce dépôt** au moment de la rédaction : ce plan en reprend tout ce dont le firmware a besoin (§1), il se suffit à lui-même.
- Le pinout de `src/pins.h` est **conservé à l'identique** par la carte. Le travail n'est donc pas un remappage, mais :
  1. une **définition de carte** PlatformIO correcte (celle d'aujourd'hui, `lolin_s3_mini`, est dangereuse sur Turret2) ;
  2. le **pilotage des nouveaux signaux** (ampli SD/gain, PWR_FLT, LEDs, boutons, SW1) ;
  3. une **séquence de boot ordonnée** (aujourd'hui tout démarre en vrac) ;
  4. le **portage de l'IMU** (ADXL345 → LSM6DSOX) ;
  5. la correction de **bugs existants qui ressembleront à des pannes matérielles** pendant la mise en service.

---

## 1. Référence matérielle Turret2 (ce que le firmware doit savoir)

### 1.1 Ce qui change par rapport au V4

| Bloc | Wemos S3 mini (V4) | Turret2 | Impact firmware |
|---|---|---|---|
| Module | ESP32-S3FH4R2 : 4 Mo flash, 2 Mo PSRAM | **ESP32-S3-MINI-1-N8** : 8 Mo flash quad, **pas de PSRAM** | nouvelle carte PlatformIO, partitions 8 Mo |
| IMU | ADXL345 | **LSM6DSOX**, I²C 0x6A, WHO_AM_I 0x6C, + gyroscope, INT1 sur IO37 | portage `Motion` |
| Ampli | module MAX98357A, SD/gain câblés en dur | MAX98357A sur 5 V, **SD_MODE sur IO13** (via 2 kΩ), **gain sur IO21 / IO47** | séquence mute/unmute, gain logiciel |
| Alim | 5 V direct | USB-C 5 V → **eFuse TPS259573** (limite 3,86 A, coupure > 6,0 V, soft-start, **FLT sur IO38**) → +5 V en étoile (branche servos / branche logique) ; 3V3 par buck-boost TPS631000 alimenté depuis le +5 V | surveillance défaut, délestage |
| NeoPixels | données en 3,3 V direct (scintillement) | **SN74AHCT125** en 5 V sur les trois lignes | rien ; le scintillement doit disparaître |
| Hall | alimentés en 5 V (bug du V4) | alimentés en **3,3 V** (J17 / J18) | seuils ADC à recalibrer |
| Nouveaux | — | LED verte IO33, LED rouge IO48, boutons IO26 / IO34, SW1 sur IO3, UART0 de secours (J16), port Qwiic (J11), USB déporté (J1) | nouveaux modules |

### 1.2 Table des GPIO

| GPIO | Fonction | Remarque |
|---|---|---|
| IO0 | BOOT (SW2 à la masse) | strap — ne pas utiliser |
| IO1 / IO2 | servo canon gauche / droit | *pins.h* |
| IO3 | SW1 (interrupteur DIP à la masse, pull-up 10 k R9) | strap JTAG_SEL, **sans effet tant que l'eFuse `STRAP_JTAG_SEL` n'est pas brûlé** → lisible comme GPIO. Rôle à décider (§9) |
| IO4 / IO5 | servo aile gauche / droite (servos à rotation continue : 90 = arrêt) | *pins.h* |
| IO6 / IO7 | servo rotation X / Z (positionnels) | *pins.h* |
| IO8 / IO9 | Hall gauche / droit, ADC1 | *pins.h* — capteurs en 3,3 V |
| IO10 / IO11 / IO12 | I²S DIN / BCLK / LRCLK | *pins.h* |
| IO13 | AMP_SD (SD_MODE via 2 kΩ, pull-down interne 100 k dans l'ampli) | bas = shutdown ; haut = canal gauche |
| IO14 / IO15 / IO16 | NeoPixel anneau (9 LEDs) / canon gauche (2) / canon droit (2), via AHCT125 | *pins.h* |
| IO17 / IO18 | radar HLK-LD2450 UART1 RX / TX, 256000 bauds | *pins.h* |
| IO19 / IO20 | USB natif D− / D+ | ne jamais toucher |
| IO21 | AMP_GAIN (GAIN_SLOT direct) | **jamais à l'état haut** |
| IO26 | bouton A (J12), pull-up 10 k + 100 nF, 1 kΩ série, actif bas | libre uniquement parce que le module est un N8 (sur N4R2 c'est la PSRAM) |
| IO33 | LED verte (470 Ω) | |
| IO34 | bouton B (J13), même montage que A | |
| IO35 / IO36 | I²C SDA / SCL, pull-ups 2,2 k près de l'IMU | *pins.h* — partagé avec le Qwiic |
| IO37 | IMU INT1 | réserve |
| IO38 | PWR_FLT, drain ouvert de l'eFuse, pull-up 10 k vers 3V3 | **bas = défaut** ; entrée uniquement |
| IO39–IO42 | non connectés | (ancien JTAG) |
| IO43 / IO44 | UART0 TXD0 / RXD0 sur J16 | logs de secours, flash de secours |
| IO45 / IO46 | straps, non connectés | ne pas utiliser |
| IO47 | AMP_GAIN_100K (GAIN_SLOT via 100 kΩ) | **jamais à l'état haut** |
| IO48 | LED rouge (470 Ω) | |

Gain de l'ampli selon IO21 / IO47 :

| IO21 | IO47 | Gain |
|---|---|---|
| haute impédance | haute impédance | 9 dB (défaut au reset) |
| bas | haute impédance | 12 dB |
| haute impédance | bas | 15 dB |

---

## 2. Pièges critiques — à lire avant de coder

### 2.1 Définition de carte

- **Ne pas garder `board = lolin_s3_mini`.** Elle déclare 4 Mo, ajoute `-DBOARD_HAS_PSRAM`, et son variant (`variants/lolin_s3_mini/pins_arduino.h` d'arduino-esp32) place `LED_BUILTIN` / `RGB_BUILTIN` sur **IO47 = AMP_GAIN_100K**. Le moindre `digitalWrite(LED_BUILTIN, HIGH)` ou `rgbLedWrite(RGB_BUILTIN, …)`, dans une lib ou un exemple, pousse IO47 à 3,3 V : interdit.
- **Ne pas prendre non plus le variant générique `esp32s3`** (celui d'`esp32-s3-devkitc-1`, pourtant « N8, 8 MB QD, No PSRAM ») : il définit **SDA = 8 et SCL = 9, soit les deux entrées Hall**. Tout `Wire.begin()` sans argument — ce que fait implicitement `accel.begin()` aujourd'hui (`src/sensors/Motion.cpp:6`) — mettrait l'I²C sur IO8 / IO9. Son `RGB_BUILTIN` est sur IO48 (LED rouge : sans danger, mais trompeur).
- **Solution retenue** :
  - `boards/turret2.json` : copie de `esp32-s3-devkitc-1.json` (8 Mo, `flash_mode qio`, `memory_type qio_qspi`, partitions `default_8MB.csv`), **sans `BOARD_HAS_PSRAM`**, variant `turret2` ;
  - `variants/turret2/pins_arduino.h` : TX 43, RX 44, SDA 35, SCL 36, `LED_BUILTIN` 33, **pas de `RGB_BUILTIN`** ;
  - et malgré tout **`Wire.begin(PIN_SDA, PIN_SCL)` explicite** partout.
- **Pas de PSRAM activée** : en mode quad, le driver PSRAM configure IO26 en SPICS1 → conflit avec le bouton A.
- **Jamais `opi` en `memory_type`** : le mode octal prend IO33 à IO37 (LED verte, bouton B, I²C, INT1) et le module ne démarre pas.
- Au premier flash, **prouver que c'est un N8** : `esptool.py flash_id` doit annoncer 8 MB ; au boot, `ESP.getFlashChipSize() == 8 Mo` et `ESP.getPsramSize() == 0`, sinon code d'erreur LED.

### 2.2 Broches

| GPIO | Règle |
|---|---|
| IO21, IO47 | configurer en **`OUTPUT_OPEN_DRAIN`** : '1' = haute impédance, '0' = masse. Physiquement incapables de tirer à 3,3 V — plus sûr que de basculer entre `INPUT` et `OUTPUT` |
| IO19 / IO20 | USB natif, ne jamais reconfigurer |
| IO0, IO45, IO46 | straps, ne pas utiliser |
| IO3 | lisible comme GPIO tant que `STRAP_JTAG_SEL` n'est pas brûlé |
| IO38 | entrée seulement (pull-up externe) |
| IO8 / IO9 | ADC1 → compatibles avec le Wi-Fi ; ne jamais déplacer les Hall sur l'ADC2 |

**Aucun eFuse de l'ESP32 à brûler, jamais** (`STRAP_JTAG_SEL`, `DIS_USB_JTAG`, `DIS_PAD_JTAG`, `VDD_SPI_*`…). `espefuse.py summary` uniquement, en lecture seule.

### 2.3 Ampli MAX98357A

- **Ne jamais arrêter LRCLK pendant que BCLK tourne** (tension continue en sortie, haut-parleur grillé). Mettre SD à l'état bas **avant** tout `i2s.end()`, tout changement de fréquence d'échantillonnage, tout `ESP.restart()` (y compris celui de l'OTA, `src/web/Ota.cpp`) et tout passage en défaut.
- Le gain n'est lu qu'à la sortie du shutdown : SD bas → réglage du gain → 10 ms → SD haut.
- Fréquences LRCLK autorisées : 8, 16, 32, 44,1, 48, 88,2, 96 kHz ; BCLK = 32, 48 ou 64 × LRCLK. 16 bits stéréo = 32 × fs ✓.
- Mono : avec SD_MODE à ~3,2 V (3,3 V à travers 2 kΩ contre le pull-down de 100 k), l'ampli joue le **canal gauche**. AudioTools, avec `channels = 1`, duplique l'échantillon dans les deux slots (driver legacy, `channel_format` par défaut `I2S_CHANNEL_FMT_RIGHT_LEFT`) ou le place dans le slot gauche (driver IDF 5). Dans les deux cas c'est bon — à confirmer à l'écoute. Ne pas passer en mode (L+R)/2.

### 2.4 IMU LSM6DSOX

- `Adafruit_LSM6DS::getEvent()` prend **trois** pointeurs (accéléro, gyro, température). Le README Turret2 affirme que « `getEvent()` is unchanged » : c'est faux, à corriger. Utiliser `getAccelerometerSensor()->getEvent(&e)` ou la version à trois arguments.
- La carte est montée **verticalement** : les axes ne correspondent plus à ceux de l'ADXL345 du V4 → mapping d'axes calibré (« tourelle debout ») et stocké en NVS.
- L'audio fort et les vibrations des servos provoquent de faux déclenchements : filtrer ou ignorer la détection de mouvement pendant les tirs et les déplacements.
- L'adresse 0x6A est prise sur le bus Qwiic partagé.

### 2.5 Alimentation

- L'ESP32 est alimenté **derrière** l'eFuse (3V3 tiré du +5 V). Si l'eFuse coupe complètement, l'ESP s'éteint aussi. PWR_FLT n'est donc lisible que pendant un affaissement du 5 V (limitation de courant) tant que le buck-boost tient le 3V3. D'où deux mécanismes complémentaires :
  - **interruption sur IO38** (front descendant) → délestage immédiat ;
  - **`esp_reset_reason()` au boot** : `ESP_RST_BROWNOUT`, ou un `ESP_RST_POWERON` inattendu, signale que l'alim a lâché.
- **À vérifier dans la datasheet TI** : TPS259573 en **latch-off ou auto-retry** ? En latch-off, après un défaut la carte reste éteinte jusqu'au débranchement USB ; en auto-retry, on peut voir une boucle de redémarrages (le compteur de brownouts la révélera).
- Sur un port USB de PC (0,5 à 0,9 A), **ne pas brancher les servos** → un mode banc qui n'attache aucun servo est utile (§9, SW1).
- Ne jamais brancher l'USB-C de la carte et l'USB déporté (J1) sur deux hôtes en même temps.

---

## 3. Ordre de boot

### 3.1 Phase 0 — matériel, avant le firmware (≈ 0 à 300 ms)

| t | Ce qui se passe | État des périphériques |
|---|---|---|
| 0 | USB branché ; soft-start de l'eFuse (dVdt 47 nF ≈ 0,9 V/ms, ~1,1 A d'appel) → +5 V en ~6 ms | servos, radar, ampli, LEDs et AHCT alimentés **en même temps** (plus de load switch) |
| ~6 ms | le TPS631000 démarre → 3V3 ; RC sur EN (10 kΩ / 1 µF) | Hall et IMU alimentés |
| ~10–15 ms | reset libéré, straps lus : IO0 = 1 → boot flash ; IO3 ignoré ; IO45 / IO46 = 0 | — |
| → ~300 ms | ROM puis bootloader ; **tous les GPIO en haute impédance** | ampli muet (pull-down interne de SD) ✓ ; gain flottant = 9 dB ✓ ; servos sans impulsion (tressautement possible) ; **entrées de l'AHCT flottantes → LEDs possiblement aléatoires** (vérifier s'il y a des pull-down sur IO14–16) ; le radar démarre |

Conséquence : le firmware doit reprendre la main sur les sorties **dans les premières millisecondes de `setup()`**, avant le `delay(1000)` actuel (`src/main.cpp:28`).

### 3.2 Phases firmware

| # | Étape | Pourquoi à cet endroit |
|---|---|---|
| 1 | **État sûr (< 5 ms)** : IO13 = `OUTPUT` bas (mute explicite) ; IO21 / IO47 en open-drain relâchés (9 dB) ; LEDs verte + rouge allumées (« boot » + test des LEDs) ; IO38, IO26, IO34, IO3 en entrée ; lecture de SW1, des boutons et de la raison du reset | fixe tout ce qui flotte ; SW1 et les boutons au boot choisissent le mode (normal / banc / remise à zéro) |
| 2 | **NeoPixels** : `FastLED.addLeds` + trame noire + plafond de puissance | efface les LEDs aléatoires et retire jusqu'à ~0,8 A du 5 V avant les servos |
| 3 | **Console** : `Serial` (USB CDC), miroir optionnel sur UART0 ; attente de l'hôte uniquement en build debug ; bannière : version, raison du reset, SW1, PWR_FLT, taille flash / PSRAM | diagnostics dès le début ; le `delay(1000)` fixe disparaît |
| 4 | **Settings (NVS)**, puis **LittleFS** (monté ici, sans formatage automatique silencieux) | tout le reste en dépend : seuils Hall, gain, volume, luminosité, offsets |
| 5 | **I²C** : `Wire.begin(35, 36, 400000)` + récupération de bus (9 coups d'horloge si SDA reste bas) + scan journalisé | l'IMU peut bloquer SDA après un reset à chaud en pleine transaction ; le scan montre aussi ce qui est branché sur le Qwiic |
| 6 | **IMU** : WHO_AM_I = 0x6C, plages (±4 g, 104 Hz), contrôle de gravité | refuser de déployer les ailes si la tourelle est couchée |
| 7 | **Radar** : `Serial1.begin(256000, SERIAL_8N1, 17, 18)` ; drapeau « radar vivant » à la première trame, alerte après ~3 s | non bloquant ; il démarre de son côté depuis la phase 0 |
| 8 | **Hall** : atténuation 11 dB (défaut), première lecture → état initial des ailes (fermées / ouvertes / inconnu) | aujourd'hui `isOpen = false` d'office, même si les ailes sont ouvertes au boot (reset en plein tir) |
| 9 | **Audio** : I²S démarré (horloges actives, DMA à zéro), gain réglé avec SD bas, 10 ms, **puis SD haut** | horloges stables avant l'unmute → pas de « plop » |
| 10 | **Wi-Fi AP + serveur web + OTA** | la calibration RF tire un pic de courant : on le place **avant** les servos pour ne pas cumuler les pics |
| 11 | **Servos un par un**, ~250 ms d'écart, PWR_FLT vérifié entre chaque : rotation Z → rotation X (au centre) → canon G → canon D (rentrés) → ailes (neutre = arrêt) | quatre servos positionnels qui sautent ensemble à leur consigne tirent 3 à 4 A, soit la limite de l'eFuse |
| 12 | **Homing** : si le Hall indique « pas fermé » → canons rentrés (déjà fait) → fermeture des ailes avec timeout | la mécanique repart d'un état connu |
| 13 | Fin de `BootState` : LED rouge éteinte (ou code de défaut), verte en battement → `Idle` | `BootState` devient réel : il n'est jamais activé aujourd'hui (`src/main.cpp:48` passe directement en `Idle`) |

Si PWR_FLT passe bas pendant les étapes 11 ou 12 → arrêt de la séquence, état `Fault`.

### 3.3 Séquence d'arrêt (OTA, reboot, défaut)

1. Ailes à l'arrêt, servos détachés.
2. SD à l'état bas (mute).
3. LEDs éteintes.
4. `i2s.end()`.
5. `ESP.restart()`.

À ajouter dans `Ota::Update` avant le `ESP.restart()`.

---

## 4. Gestion d'énergie en fonctionnement

- Au plus un démarrage de servo par fenêtre d'environ 150 ms (aile gauche puis aile droite, canons décalés).
- `FastLED.setMaxPowerInVoltsAndMilliamps(5, …)` : les 13 LEDs en blanc consomment ~0,8 A.
- Gain par défaut 9 dB, volume logiciel en réglage ; l'ampli peut tirer ~1,3 A crête pendant que les canons bougent.
- PWR_FLT bas (interruption) → délestage immédiat : ailes à l'arrêt, canons détachés, LEDs éteintes, ampli muet, LED rouge, état `Fault` ; reprise après 2 s de FLT haut.
- Raison du reset BROWNOUT → compteur en NVS, affiché sur la console et sur une page web `/status`.
- Option : détacher les servos de rotation au repos (moins de bourdonnement, de courant et de bruit pour l'IMU).

---

## 5. Bugs existants à corriger en premier

Ils vont se manifester pendant la mise en service et ressembler à des pannes de la carte.

| Où (commit `020a839`) | Problème | Ce qu'on verra sur la carte |
|---|---|---|
| `src/audio/AudioLoop.cpp:8` | `sizeof(samplesIn)` = taille d'un pointeur → `totalSampleCount = 2` → la lecture s'arrête après 2 échantillons | **aucun son de tir** → on accusera l'ampli ou SD_MODE |
| `src/audio/Audio.cpp:32-33` | `availableForWrite()` peut dépasser les 4096 octets de `sampleBuffer` | débordement mémoire, crashs aléatoires |
| `src/states/BootState.cpp:8-11` | LittleFS n'est monté que dans `BootState`, jamais activé ; formatage automatique si le montage échoue | `fire.mp3` introuvable, ou fichiers effacés silencieusement |
| `src/audio/ESP32Downloader.cpp:4` | inclut `config.h`, absent du dépôt ; `WiFiClientSecure` sans son include ; fonction jamais appelée | probable erreur de compilation → exclure ou supprimer |
| `src/sensors/Motion.cpp:6-15` | échec de `begin()` ignoré, puis lecture I²C à chaque tour de boucle | boucle ralentie → audio haché, servos saccadés |
| `src/states/IdleState.cpp:14-15` | trois prints par tour de boucle | USB CDC saturé, boucle ralentie quand un terminal est ouvert |
| `src/motion/Wing.cpp:71,79` | seuils Hall 2500 / 1500 codés en dur | ailes arrêtées seulement au timeout de 2 s |
| `src/motion/Wing.cpp` (`write(90)`) | avec la plage 500–2400 µs, `write(90)` = 1450 µs, pas 1500 | les ailes (servos continus) peuvent glisser → trim de neutre en réglage |
| `platformio.ini` | plateforme `espressif32` et libs non figées (FastLED, ESPAsyncWebServer, audio-tools en HEAD git) | build qui casse sans prévenir ; AudioTools change de driver I²S selon la version d'IDF |

---

## 6. Architecture proposée

```
boards/turret2.json                  carte PlatformIO (8 Mo, QIO, sans PSRAM)
variants/turret2/pins_arduino.h      SDA 35 / SCL 36 / TX 43 / RX 44 / LED_BUILTIN 33
src/pins.h                           commun + bloc #ifdef BOARD_TURRET2 (IO13, 21, 47, 38, 33, 48, 26, 34, 3, 37)
src/board/Board.{h,cpp}              état sûr, raison du reset, LEDs d'état, boutons (anti-rebond, appui long), SW1, surveillance PWR_FLT
src/audio/Amp.{h,cpp}                SD + gain en open-drain, séquences mute/unmute, SafeShutdown()
src/sensors/Motion.*                 LSM6DSOX (ADXL345 conservé sous #ifdef pour le V4)
src/motion/Gantry.*                  attache échelonnée, homing, trims
src/states/BootState, FaultState     boot réel + état de défaut
src/web/…                            /status (raison du reset, FLT, IMU, radar, Hall, version)
```

`platformio.ini` : `env:turret2`, `env:turret2_ota`, `env:turret2_bringup` (`CORE_DEBUG_LEVEL=3` + console) ; `env:lolin_s3_mini` conservé pour les V4 (si la double cible est retenue, §9).

Partitions `default_8MB.csv` : nvs 20 Ko, app0 / app1 2 × 3,2 Mo (OTA), LittleFS (`spiffs`) 1,5 Mo, coredump 64 Ko.

Nouveaux réglages (clés NVS de 15 caractères maximum, même ordre que l'enum `SettingId`) : `HallOpenL/R`, `HallCloseL/R`, `WingTrimL/R`, `AmpGain` (9 / 12 / 15), `Volume`, `LedBright`, `LedMaxmA`, `ServoStagger`, `ImuUpAxis`.

---

## 7. Lots de travail (un commit chacun, dans cet ordre)

| Lot | Contenu | Critère de fin |
|---|---|---|
| 1 | **Base de build** : installer PlatformIO, compiler le code actuel (env `lolin_s3_mini`), corriger ce qui ne compile pas, figer plateforme + libs ; relever flash / RAM | build reproductible, versions figées dans `platformio.ini` |
| 2 | **Cible Turret2** : `boards/turret2.json`, variant, partitions 8 Mo, envs, `pins.h` | `pio run -e turret2` passe |
| 3 | **Bugs du §5** | chaque bug corrigé, build des deux cibles |
| 4 | **Module Board** : état sûr, LEDs, boutons, SW1, PWR_FLT, raison du reset | fonctions testables depuis la console |
| 5 | **Séquence de boot** : réécriture de `setup()` + `BootState` réel selon le §3 | ordre du §3.2 respecté, journalisé à la console |
| 6 | **IMU LSM6DSOX** (+ `lib_deps`), scan I²C, orientation | lecture accéléro / gyro, contrôle « debout » |
| 7 | **Audio** : module Amp, gain / volume en réglages, arrêt sûr avant reboot / OTA | pas de plop au boot, au mute, au reboot |
| 8 | **Mouvement** : attache échelonnée, homing, seuils et trims en réglages, détection Hall incohérent (bloqué à 0 ou 4095, ou aucune variation pendant un mouvement) | cycle ailes / canons fiable |
| 9 | **Énergie** : délestage sur FLT, plafond LEDs, état `Fault`, compteur brownout, `/status` | défaut simulé (IO38 à la masse) → délestage |
| 10 | **Console de mise en service** (reprend l'idée commentée de `ManualState`) : `scan`, `imu`, `hall`, `servo <n> <angle\|off>`, `led <canal> <couleur>`, `tone <Hz>`, `gain 9\|12\|15`, `mute`, `flt`, `reset-reason` | utilisable pour le §8 |
| 11 | **Docs** : section Firmware du README (correction `getEvent`, procédure de flash) | — |

---

## 8. Mise en service de la première carte

Périphériques branchés un par un ; chaque étape doit passer avant la suivante.

| Étape | Branché | Ce qu'on vérifie |
|---|---|---|
| A | rien, alimentation via un wattmètre USB | VBUS, 5 V, 3V3 sur les points de test ; courant au repos ; IO38 à 3,3 V |
| B | — | `esptool.py flash_id` = **8 MB** ; `espefuse.py summary` (lecture seule) ; flash via USB natif (sinon BOOT + RESET, sinon UART0 / J16) ; `erase_flash`, puis `upload` + `uploadfs` |
| C | — | firmware de mise en service : LEDs, boutons, SW1, PWR_FLT, bannière |
| D | — | scan I²C → 0x6A ; ~9,8 m/s² sur l'axe vertical ; choix de l'axe |
| E | haut-parleur | tonalité 1 kHz à faible volume ; gains 9 / 12 / 15 dB ; **aucun plop** au boot, au mute, au reboot OTA |
| F | anneau puis canons (LEDs seules) | chaque canal séparément, luminosité plafonnée, plus de scintillement |
| G | capteurs Hall | valeurs brutes aimant près / loin → réglage des seuils |
| H | servos, un par un puis tous | courant crête au wattmètre ; PWR_FLT ; aucun reset ; cas pire : 6 servos + audio + LEDs |
| I | radar | trames reçues, cibles cohérentes |
| J | tout | cycle complet Idle → Activate → Firing → Disengage ; OTA ; USB déporté J1 (jamais deux hôtes à la fois) |

---

## 9. Décisions à prendre

| # | Sujet | Proposition |
|---|---|---|
| D1 | **SW1** (IO3) | mode « banc / maintenance » : fermé au boot → aucun servo attaché, console active, logs verbeux ; ouvert = normal. Ou non monté |
| D2 | **Double cible** V4 / Turret2 | garder la compatibilité V4 (quelques `#ifdef`) — recommandé |
| D3 | **Boutons A / B** | A = cycle de démo ; B = mute ; A + B au boot = remise à zéro des réglages ; B appui long = Wi-Fi AP on / off |
| D4 | **Codes LED** | verte = battement (boucle vivante) ; rouge = nombre de clignotements : 1 brownout au dernier reset, 2 défaut eFuse, 3 IMU absente, 4 radar muet, 5 Hall incohérent, 6 LittleFS |
| D5 | **eFuse** TPS259573 | confirmer latch-off ou auto-retry dans la datasheet |
| D6 | **Servos au repos** | maintenir le couple ou détacher |
| D7 | **Sécurité** | l'AP « Portal Turret » est ouvert et `/update` accepte n'importe quel firmware sans authentification → mot de passe AP ? |

Les décisions prises sont reportées dans le journal ([firmware-journal.md](firmware-journal.md)), section « Décisions ».
