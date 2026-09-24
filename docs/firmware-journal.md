# Journal de bord — firmware Turret2

Ce journal accompagne [firmware-plan.md](firmware-plan.md). Il contient **tout** ce qui a été fait, ce qui est prévu, ce qui est terminé, les décisions et **les erreurs**, afin que quelqu'un qui ne connaît que le plan puisse reprendre le travail sans rien perdre.

## Mode d'emploi

- **Pour reprendre le travail** : lire « État actuel », puis « Avancement des lots », puis la dernière entrée de « Sessions ».
- **À chaque session** : mettre à jour « État actuel », « Avancement des lots », « Décisions », « Erreurs, impasses et pièges », et ajouter une entrée datée dans « Sessions ». Le journal est mis à jour dans le même commit que le travail qu'il décrit, et toujours avant un push.
- Les entrées de session passées ne sont **jamais réécrites** : une correction fait l'objet d'une nouvelle entrée qui renvoie à l'ancienne.
- Dates au format JJ.MM.AAAA. Statuts : *à faire*, *en cours*, *fait*, *bloqué*, *abandonné*.

---

## État actuel — mis à jour le 24.09.2026

- **Phase** : plan rédigé et poussé ; **aucune ligne de firmware modifiée**. Le code de `src/` est le firmware upstream intact (commit `020a839`).
- **Branche de travail** : `claude/admiring-bell-q3hube` (dépôt `lo26lo/portal-turret-v4`).
- **Prochaine action** : lot 1 du plan (base de build) — installer PlatformIO dans le conteneur, compiler l'env `lolin_s3_mini` tel quel, relever les erreurs, figer les versions.
- **Décisions ouvertes** : D1 à D7 du plan §9 (aucune tranchée).
- **Matériel** : la carte Turret2 n'est pas encore fabriquée → la mise en service (plan §8) attend les cartes ; tout le reste peut avancer sans elles.
- **Blocages** : aucun. Limites connues de l'environnement : voir « Erreurs, impasses et pièges ».

## Avancement des lots

| Lot | Contenu (plan §7) | Statut | Commit / remarque |
|---|---|---|---|
| — | Plan + journal + `CLAUDE.md` | fait | commit du 24.09.2026 qui ajoute ces fichiers |
| 1 | Base de build, versions figées | à faire | PlatformIO absent du conteneur au 24.09 |
| 2 | Cible Turret2 (board JSON, variant, partitions, envs, `pins.h`) | à faire | |
| 3 | Bugs existants (plan §5) | à faire | |
| 4 | Module Board (état sûr, LEDs, boutons, SW1, PWR_FLT, raison du reset) | à faire | dépend de D1, D3, D4 |
| 5 | Séquence de boot + `BootState` réel | à faire | |
| 6 | IMU LSM6DSOX, scan I²C, orientation | à faire | |
| 7 | Audio : module Amp, gain / volume, arrêt sûr | à faire | |
| 8 | Mouvement : attache échelonnée, homing, seuils, trims | à faire | dépend de D6 |
| 9 | Énergie : délestage, `Fault`, compteur brownout, `/status` | à faire | dépend de D5 |
| 10 | Console de mise en service | à faire | |
| 11 | Docs (README Firmware) | à faire | |

## Décisions

| Date | Sujet | Décision | Raison |
|---|---|---|---|
| 24.09.2026 | Langue et format | plan et journal en français, dans `docs/`, dates JJ.MM.AAAA | langue de travail de l'utilisateur ; même convention de date que la doc matérielle Turret2 |
| 24.09.2026 | Mémoire entre sessions | la règle du journal est écrite dans `CLAUDE.md` à la racine du dépôt | le conteneur est éphémère : seule une instruction versionnée dans le dépôt est relue automatiquement à chaque session |
| 24.09.2026 | Documentation matérielle | non ajoutée au dépôt ; le plan §1 en reprend l'essentiel | les fichiers Turret2 (README, `design-plan.md`, `status-and-history.md`, KiCad) appartiennent au travail local de l'utilisateur ; les pousser risquait des conflits avec sa version |
| — | D1 à D7 (plan §9) | ouvertes | en attente de l'utilisateur |

## Erreurs, impasses et pièges rencontrés

| Date | Quoi | Conséquence / solution |
|---|---|---|
| 24.09.2026 | Premier essai de récupération des drivers I²S d'AudioTools avec des noms de fichiers devinés (`I2SESP32.h`, `I2SESP32V1.h`) | fichiers 404 de 14 octets. Bons chemins sur `raw.githubusercontent.com/pschatzmann/arduino-audio-tools/main/` : `src/AudioTools/CoreAudio/AudioI2S/I2SDriverESP32.h` (driver legacy), `I2SDriverESP32V1.h` (IDF 5), `I2SConfigESP32.h` |
| 24.09.2026 | `api.github.com` refuse les dépôts hors du périmètre de la session | utiliser `raw.githubusercontent.com` (autorisé) pour lire des fichiers de libs publiques |
| 24.09.2026 | `www.ti.com` bloqué par le proxy (403) | datasheet TPS2595 non consultée → D5 (latch-off ou auto-retry) reste ouverte ; à vérifier par l'utilisateur ou via une autre source |
| 24.09.2026 | PlatformIO absent du conteneur | rien n'a été compilé ; l'erreur de compilation supposée sur `config.h` (`src/audio/ESP32Downloader.cpp:4`) est à confirmer au lot 1 |
| 24.09.2026 | La documentation Turret2 citée par le README (`hardware/Turret2/`, `docs/design-plan.md`, `docs/status-and-history.md`) n'existe pas sur la branche distante | travail fait à partir des trois fichiers fournis par l'utilisateur ; le plan §1 en garde tout ce qui concerne le firmware |
| 24.09.2026 | Piège documentaire : le README Turret2 affirme que `getEvent()` est inchangé avec le LSM6DSOX | faux : `Adafruit_LSM6DS::getEvent(accel, gyro, temp)` prend trois pointeurs (vérifié dans `Adafruit_LSM6DS.h`) → plan §2.4, lot 11 |

---

## Sessions

### 24.09.2026 — Analyse, plan, journal

**Demande de l'utilisateur**
1. À partir de la documentation Turret2 (README, `design-plan.md`, `status-and-history.md`) et du code existant : un plan d'adaptation du firmware au nouveau matériel — ordre de boot des périphériques, points d'attention, tout ce qui paraît utile. Plan seulement, pas de code.
2. Ensuite : pousser le plan, et tenir un journal de bord complet (fait, prévu, terminé, erreurs), mémorisé comme règle permanente.

**Fait**
- Lecture intégrale des trois documents Turret2 et de tout `src/` (`main.cpp`, `pins.h`, états, audio, capteurs, mouvement, lumière, réglages, web / OTA) et de `platformio.ini`.
- Vérifications en ligne (sources primaires) :
  - `arduino-esp32` 2.0.17, `variants/lolin_s3_mini/pins_arduino.h` : `LED_BUILTIN = 47 + SOC_GPIO_PIN_COUNT`, `RGB_BUILTIN = LED_BUILTIN` → IO47 ; SDA 35, SCL 36 ; TX 43, RX 44.
  - `platform-espressif32`, `boards/lolin_s3_mini.json` : `-DBOARD_HAS_PSRAM`, 4 MB, `memory_type qio_qspi`.
  - `arduino-esp32` 2.0.17, `variants/esp32s3/pins_arduino.h` : **SDA 8, SCL 9**, `RGB_BUILTIN` sur `PIN_NEOPIXEL` (48).
  - `boards/esp32-s3-devkitc-1.json` : « ESP32-S3-DevKitC-1-N8 (8 MB QD, No PSRAM) », `default_8MB.csv`, variant `esp32s3`.
  - `tools/partitions/default_8MB.csv` : nvs 0x5000, otadata, app0 / app1 0x330000, spiffs 0x180000, coredump 0x10000.
  - `Adafruit_LSM6DS.h` : `getEvent(accel, gyro, temp)`, adresse par défaut 0x6A ; `Adafruit_LSM6DSOX.h` : chip id 0x6C.
  - AudioTools : driver legacy, `channel_format` par défaut `I2S_CHANNEL_FMT_RIGHT_LEFT`, le mono est dupliqué dans les deux slots (`writeExpandChannel`) ; driver IDF 5, mono → `slot_mask = I2S_STD_SLOT_LEFT`. Les deux conviennent au MAX98357A en mode canal gauche.
- Relevé des bugs existants qui imiteront des pannes matérielles (plan §5), notamment `AudioLoop` (`sizeof` d'un pointeur → pas de son de tir) et LittleFS jamais monté (`BootState` jamais activé).
- Plan présenté à l'utilisateur, puis écrit dans `docs/firmware-plan.md` (version complète, avec la référence GPIO du §1 pour qu'il se suffise à lui-même).
- Création de ce journal et de `CLAUDE.md` (règle permanente du journal + règles matérielles critiques).

**Non fait / reporté**
- Aucune compilation (PlatformIO absent).
- Datasheet TPS2595 non consultée (proxy).

**Prochaine étape**
- Lot 1 : `pip install platformio`, `pio run -e lolin_s3_mini`, noter chaque erreur ici, figer `platform = espressif32@<version>` et les versions des libs qui compilent.
- Faire trancher par l'utilisateur au moins D1 (SW1), D2 (double cible), D3 (boutons) et D4 (codes LED) avant le lot 4.
