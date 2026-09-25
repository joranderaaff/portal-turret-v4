# Journal de bord — firmware Turret2

Ce journal accompagne [firmware-plan.md](firmware-plan.md). Il contient **tout** ce qui a été fait, ce qui est prévu, ce qui est terminé, les décisions et **les erreurs**, afin que quelqu'un qui ne connaît que le plan puisse reprendre le travail sans rien perdre.

## Reprise rapide

Pour reprendre le travail sans autre contexte :

1. Lire `CLAUDE.md` (règles permanentes), puis « État actuel » ci-dessous : il donne la phase, la **prochaine action** et les blocages.
2. Lire dans « Avancement des lots » le premier lot qui n'est pas *fait*, puis sa description dans [firmware-plan.md](firmware-plan.md) §7 (et les sections qu'elle cite).
3. Parcourir « Erreurs, impasses et pièges » avant de lancer une commande : on n'y retombe pas deux fois.
4. Travailler sur la branche indiquée dans « État actuel » ; à la fin, mettre à jour ce journal **dans le même commit** que le travail, puis pousser.

Documents du dépôt : [firmware-plan.md](firmware-plan.md) (plan firmware), [design-plan.md](design-plan.md) et [status-and-history.md](status-and-history.md) (matériel, font foi sur la carte), [../README.md](../README.md) (présentation de la carte).

## Mode d'emploi

- **Pour reprendre le travail** : lire « État actuel », puis « Avancement des lots », puis la dernière entrée de « Sessions ».
- **À chaque session** : mettre à jour « État actuel », « Avancement des lots », « Décisions », « Erreurs, impasses et pièges », et ajouter une entrée datée dans « Sessions ». Le journal est mis à jour dans le même commit que le travail qu'il décrit, et toujours avant un push.
- Les entrées de session passées ne sont **jamais réécrites** : une correction fait l'objet d'une nouvelle entrée qui renvoie à l'ancienne.
- Dates au format JJ.MM.AAAA. Statuts : *à faire*, *en cours*, *fait*, *bloqué*, *abandonné*.

---

## État actuel — mis à jour le 25.09.2026

- **Phase** : plan terminé, **décisions D1 à D7 tranchées** (plan §9), page web de configuration ajoutée au plan (§10, lot 12). Documentation matérielle Turret2 ajoutée au dépôt (`README.md`, `docs/design-plan.md`, `docs/status-and-history.md`) ; le projet KiCad n'y est pas. **Aucune ligne de firmware modifiée** : `src/` est le firmware upstream intact (commit `020a839`).
- **Cible** : Turret2 uniquement (plus de compatibilité Wemos / V4, D2).
- **Branche de travail** : `claude/admiring-bell-q3hube` (dépôt `lo26lo/portal-turret-v4`).
- **Prochaine action** : lot 1 du plan (base de build) — installer PlatformIO dans le conteneur, compiler une dernière fois l'env `lolin_s3_mini` tel quel pour avoir la référence (erreurs, tailles), figer les versions. Puis lot 2 (cible Turret2, suppression des envs Wemos).
- **Décisions ouvertes** : aucune. Reste à vérifier sans urgence : variante latch-off / auto-retry du TPS259573 (le firmware gère les deux, D5).
- **Matériel** : la carte Turret2 n'est pas encore fabriquée → la mise en service (plan §8) attend les cartes ; tout le reste peut avancer sans elles.
- **Blocages** : aucun. Limites connues de l'environnement : voir « Erreurs, impasses et pièges ».

## Avancement des lots

| Lot | Contenu (plan §7) | Statut | Commit / remarque |
|---|---|---|---|
| — | Plan + journal + `CLAUDE.md` | fait | commit du 24.09.2026 qui ajoute ces fichiers |
| 1 | Base de build, versions figées | à faire | PlatformIO absent du conteneur au 24.09 ; dernière compilation `lolin_s3_mini` pour référence |
| 2 | Cible Turret2 (board JSON, variant, partitions, envs, `pins.h`) + suppression des envs Wemos | à faire | D2 |
| 3 | Bugs existants (plan §5) | à faire | |
| 4 | Module Board (état sûr, LEDs, boutons, SW1, PWR_FLT, raison du reset, boucle de redémarrage) | à faire | D1, D3, D4, D5 tranchées |
| 5 | Séquence de boot + `BootState` réel | à faire | |
| 6 | IMU LSM6DSOX, scan I²C, orientation | à faire | |
| 7 | Audio : module Amp, gain / volume, arrêt sûr | à faire | |
| 8 | Mouvement : attache échelonnée, homing, seuils, trims, maintien des servos | à faire | D6 tranchée |
| 9 | Énergie : délestage, `Fault`, compteur brownout, mode réduit | à faire | D5 tranchée |
| 10 | Console de mise en service | à faire | |
| 11 | Docs (README Firmware) | à faire | |
| 12 | Page web de configuration (plan §10) | à faire | demandée le 24.09 ; partie « réglages » avançable après le lot 3 |

## Décisions

| Date | Sujet | Décision | Raison |
|---|---|---|---|
| 24.09.2026 | Langue et format | plan et journal en français, dans `docs/`, dates JJ.MM.AAAA | langue de travail de l'utilisateur ; même convention de date que la doc matérielle Turret2 |
| 24.09.2026 | Mémoire entre sessions | la règle du journal est écrite dans `CLAUDE.md` à la racine du dépôt | le conteneur est éphémère : seule une instruction versionnée dans le dépôt est relue automatiquement à chaque session |
| 24.09.2026 | Documentation matérielle | non ajoutée au dépôt ; le plan §1 en reprend l'essentiel | les fichiers Turret2 (README, `design-plan.md`, `status-and-history.md`, KiCad) appartiennent au travail local de l'utilisateur ; les pousser risquait des conflits avec sa version |
| 24.09.2026 | D1 SW1 | mode banc, pour usage futur (aucun servo attaché automatiquement, machine d'états arrêtée, console, logs verbeux) | choix de l'utilisateur |
| 24.09.2026 | D2 compatibilité | Turret2 uniquement, plus de Wemos / V4 : pas de `#ifdef`, envs `lolin_s3_mini*` et ADXL345 supprimés | choix de l'utilisateur ; simplifie le code |
| 24.09.2026 | D3 boutons | A = démo, B = mute, A + B au boot = remise à zéro, B 3 s = Wi-Fi on / off | proposition acceptée |
| 24.09.2026 | D4 codes LED | verte = battement ; rouge = 1 brownout / boucle, 2 eFuse, 3 IMU, 4 radar, 5 Hall, 6 LittleFS | proposition acceptée |
| 24.09.2026 | D5 eFuse | le firmware gère latch-off et auto-retry sans connaître la variante : détection de boucle de redémarrage (3 resets de suite) → mode réduit | l'utilisateur a laissé le choix (« fais au mieux ») ; datasheet inaccessible d'ici |
| 24.09.2026 | D6 servos au repos | ailes détachées à l'arrêt ; canons détachés ~500 ms après leur mouvement ; rotation maintenue ailes ouvertes, détachée après `ServoIdleMs` (5 s) en Idle, ré-attachée sur la dernière consigne | l'utilisateur a laissé le choix ; détacher les servos continus supprime le glissement du neutre, et moins de servos alimentés = moins de courant et de bruit pour l'IMU |
| 24.09.2026 | D7 sécurité | AP en WPA2, mot de passe `ApPassword` (défaut `stillalive`), même mot de passe en HTTP Basic sur l'API et `/update` ; A + B au boot le réinitialise | accepté par l'utilisateur ; défaut connu mais récupérable, avertissement dans la page tant qu'il n'est pas changé |
| 24.09.2026 | Documentation matérielle (révise la décision « non ajoutée au dépôt » ci-dessus) | ajoutée telle quelle : README Turret2 → `README.md` (le dépôt n'en avait pas), `docs/design-plan.md`, `docs/status-and-history.md`. Aucune modification de leur contenu | demande de l'utilisateur ; emplacements indiqués par le README lui-même |
| 24.09.2026 | Page web | page de configuration complète embarquée dans le firmware (gzip PROGMEM), générée depuis la liste des réglages, commandes passées à `loop()` par une file | demande de l'utilisateur ; vérifié qu'elle n'existait pas (seulement `GET /` et `GET /settings` en JSON, lecture seule) |

## Erreurs, impasses et pièges rencontrés

| Date | Quoi | Conséquence / solution |
|---|---|---|
| 24.09.2026 | Premier essai de récupération des drivers I²S d'AudioTools avec des noms de fichiers devinés (`I2SESP32.h`, `I2SESP32V1.h`) | fichiers 404 de 14 octets. Bons chemins sur `raw.githubusercontent.com/pschatzmann/arduino-audio-tools/main/` : `src/AudioTools/CoreAudio/AudioI2S/I2SDriverESP32.h` (driver legacy), `I2SDriverESP32V1.h` (IDF 5), `I2SConfigESP32.h` |
| 24.09.2026 | `api.github.com` refuse les dépôts hors du périmètre de la session | utiliser `raw.githubusercontent.com` (autorisé) pour lire des fichiers de libs publiques |
| 24.09.2026 | `www.ti.com` bloqué par le proxy (403) | datasheet TPS2595 non consultée → D5 (latch-off ou auto-retry) reste ouverte ; à vérifier par l'utilisateur ou via une autre source |
| 24.09.2026 | PlatformIO absent du conteneur | rien n'a été compilé ; l'erreur de compilation supposée sur `config.h` (`src/audio/ESP32Downloader.cpp:4`) est à confirmer au lot 1 |
| 24.09.2026 | La documentation Turret2 citée par le README (`hardware/Turret2/`, `docs/design-plan.md`, `docs/status-and-history.md`) n'existe pas sur la branche distante | travail fait à partir des trois fichiers fournis par l'utilisateur ; les deux `docs/` et le README ont été ajoutés en 3ᵉ session ; `hardware/Turret2/` reste absent (les liens du README vers ce dossier sont donc cassés sur GitHub) |
| 24.09.2026 | Bugs trouvés dans `Settings` en préparant la page web : paramètre `group` ignoré (`Settings.cpp:10,12`), `Settings::SetFromString` déclarée sans définition (`Settings.h:77`), copie par valeur de `Settings` dans `TurretWebServer.cpp:60` | ajoutés au plan §5, corrigés au lot 12 |
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

### 24.09.2026 (2ᵉ session) — Décisions et page web

**Demande de l'utilisateur** : réponses aux décisions du plan §9 — (1) SW1 en mode banc pour usage futur, (2) pas de compatibilité Wemos, (3) et (4) propositions boutons et LEDs acceptées, (5) et (6) « fais au mieux », (7) oui au mot de passe ; ajout (8) : une page web où l'on peut tout configurer, si elle n'existe pas.

**Fait**
- Vérifié dans le code que la page n'existe pas : `TurretWebServer.cpp` ne sert que `GET /` et `GET /settings` (JSON, lecture seule) ; `Ota.cpp` sert `POST /update`.
- En vérifiant, relevé trois bugs dans les réglages (voir « Erreurs ») et ajouté au plan §5.
- Plan mis à jour : en-tête, §1.2 (IO3 = mode banc), §4 (détection de boucle de redémarrage, D5), §5 (bugs `Settings`), §6 (architecture sans `#ifdef`, couche `Actions`, page embarquée, nouveaux réglages `ServoIdleMs`, `ApSsid`, `ApPassword`), §7 (lots 1, 2, 6 ajustés, lot 12, ordre conseillé), §9 réécrit avec les décisions, nouveau §10 (page web : principes, API, contenu, pièges).
- Choix faits sur délégation de l'utilisateur (D5, D6) et valeur par défaut du mot de passe (D7) : voir « Décisions ».

**Non fait** : toujours aucune compilation (lot 1 pas commencé).

**Prochaine étape** : lot 1.

### 24.09.2026 (3ᵉ session) — Documentation matérielle dans le dépôt

**Demande de l'utilisateur** : pousser aussi le README, `status-and-history` et `design-plan`.

**Fait**
- Copie sans modification des trois fichiers fournis au début de la 1ʳᵉ session : README Turret2 → `README.md` à la racine (il n'y en avait pas), `docs/design-plan.md`, `docs/status-and-history.md` — emplacements donnés par la section « Repository layout » du README.
- Plan §0 : lien vers ces documents ; la doc matérielle fait foi sur les points matériels.
- Décision « documentation non ajoutée » de la 1ʳᵉ session révisée (nouvelle ligne dans « Décisions »).

**À savoir**
- Le README (en anglais) contient encore l'affirmation fausse sur `getEvent()` et une section Firmware antérieure au plan : laissé tel quel, à corriger au lot 11.
- `hardware/Turret2/` (projet KiCad) n'est pas dans le dépôt : les liens du README vers ce dossier ne mènent nulle part tant que l'utilisateur ne l'a pas poussé.

**Prochaine étape** : lot 1.

### 25.09.2026 — Reprise rapide

**Demande de l'utilisateur** : « et le journal de reprise ? » — vérifier que le journal est bien disponible pour reprendre le travail.

**Fait**
- Vérifié que `docs/firmware-journal.md` est sur la branche distante `claude/admiring-bell-q3hube` (commits `c17977f`, `83e856e`, `73ab93d`).
- Ajout en tête d'une section « Reprise rapide » (procédure en 4 étapes + liste des documents).
- Correction : le titre « État actuel » indiquait « 2ᵉ session » alors qu'il avait été mis à jour en 3ᵉ session ; il porte maintenant la date du 25.09.2026.

**Prochaine étape** : lot 1 (inchangée).

