# Instructions pour Claude — firmware de la tourelle Portal, carte Turret2

Ce dépôt contient le firmware (PlatformIO, Arduino, ESP32-S3) de la tourelle Portal V4, en cours d'adaptation à la carte **Turret2**. Langue de travail : **français**. Dates au format JJ.MM.AAAA.

## Journal de bord — obligatoire, à chaque session

Le journal est `docs/firmware-journal.md`. Il doit permettre à quelqu'un qui ne connaît que le plan (`docs/firmware-plan.md`) de tout savoir et de continuer le travail.

- **En début de session** : lire `docs/firmware-journal.md` (« État actuel », « Avancement des lots », dernière session), puis les sections utiles de `docs/firmware-plan.md`.
- **Y consigner tout** : ce que tu fais, ce que tu prévois de faire, ce qui est terminé, **les erreurs** (y compris les tiennes : commandes ratées, hypothèses fausses, impasses, retours en arrière), les décisions avec leur raison, les faits vérifiés avec leur source, ce qui a été testé et comment.
- **Tenir à jour** : « État actuel », « Avancement des lots », « Décisions », « Erreurs, impasses et pièges », plus une entrée datée dans « Sessions ». Ne jamais réécrire une entrée de session passée : corriger par une nouvelle entrée.
- **Commit** : le journal est mis à jour dans le même commit que le travail qu'il décrit (ou dans le commit qui suit immédiatement), et **toujours avant un push**. Jamais de code poussé avec un journal en retard.

## Références

- `docs/firmware-plan.md` : contexte, référence matérielle (GPIO), pièges, ordre de boot, lots de travail, décisions à prendre.
- `docs/firmware-journal.md` : avancement réel.
- `src/pins.h` : pinout imposé ; la carte Turret2 est câblée dessus, **ne pas réaffecter ces broches**.

## Règles matérielles critiques (détails : plan §2)

- **IO21 et IO47 ne doivent jamais être à l'état haut** (gain de l'ampli, détection comparée à 5 V) : uniquement `OUTPUT_OPEN_DRAIN` ou entrée.
- Ne pas utiliser la carte PlatformIO `lolin_s3_mini` pour Turret2 (4 Mo, PSRAM, LED intégrée sur IO47), ni les broches par défaut du variant générique `esp32s3` (SDA/SCL = 8/9 = capteurs Hall). Toujours `Wire.begin(35, 36)` explicite.
- Module ESP32-S3-MINI-1-**N8** : pas de PSRAM activée (IO26 = bouton A), jamais `memory_type` `opi` (IO33–IO37).
- Ne jamais brûler d'eFuse de l'ESP32 (`espefuse.py` en lecture seule uniquement).
- Ne jamais utiliser IO0, IO19, IO20, IO45, IO46.
- Couper l'ampli (IO13 bas) avant tout `i2s.end()`, changement de fréquence d'échantillonnage ou `ESP.restart()` : ne jamais arrêter LRCLK pendant que BCLK tourne.
