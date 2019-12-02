# Progetto ESP32 Firmware

## Table of Contents

- [Progetto ESP32 Firmware](#progetto-esp32-firmware)
  - [Table of Contents](#table-of-contents)
  - [Bootstrapping](#bootstrapping)
  - [Apertura Progetto](#apertura-progetto)
  - [Build del Progetto](#build-del-progetto)
  - [Flash del Progetto](#flash-del-progetto)
  - [Monitor Applicazione](#monitor-applicazione)
  - [Quick Fix per tutti i problemi](#quick-fix-per-tutti-i-problemi)
  - [Formattazione totale ESP32](#formattazione-totale-esp32)

**IDE Utilizzato**: [CLion](https://www.jetbrains.com/clion/) Open Source

## Bootstrapping

**Questa parte è molto importante e necessaria per poter avviare qualsiasi codice sorgente presente nella repository per quanto riguarda ESP32. Attenersi in maniera categorica a tutte le istruzioni riportate in documentazione.**

Per poter sviluppare, testare o implementare nuove funzioni si consiglia di seguire le istruzioni che trovate nella [documentazione ufficiale CMAKE](https://docs.espressif.com/projects/esp-idf/en/latest/get-started-cmake/index.html), in particolare dal punto **1** al punto **4**.

In queste istruzioni si suppone che il progetto sia già stato clonato nella sua interezza sul PC e che sia stata installata correttamente la [**Toolchain**](https://docs.espressif.com/projects/esp-idf/en/latest/get-started-cmake/index.html#installation-step-by-step) per lo sviluppo.

Nota: Nella sezione di Toolchain Setup, della guida sopra citata, si fa riferimento al possibile errore '**Permission issues /dev/ttyUSB0**'. Se il procedimento indicato non dovesse risolvere il problema, usare il comando:

```bash
sudo chmod 777 /dev/ttyUSB0
```

## Apertura Progetto

Aprire la cartella **ESP32FirmwareCLion** in Clion attraverso `File -> Open...`

## Build del Progetto

1. Aprire il terminale di Clion premendo su `Terminal` in basso a destra.
2. Avviare il comando `idf.py build`

Il progetto verrà buildato e verranno mostrati warning ed errori, se trovati.

## Flash del Progetto

1. Aprire il terminale di CLion
2. Avviare il comando `idf.py flash`

Con questo comando ci occupiamo di flashare il nostro firmware, ma anche di buildare e flashare l'immagine del captive portal a partire da ciò che è contenuto nella cartella `captive_portal`!

## Monitor Applicazione

1. Aprire il terminale di CLion
2. Avviare il comando `idf.py -b 115200 monitor`

## Quick Fix per tutti i problemi

Se riscontrate qualche problema, prima di mettervi le mani nei capelli:

- Avviate da terminale: `idf.py clean` e `idf.py fullclean`.
- Da Clion andate su `File -> Reload CMake project`
- **Mettetevi le mani nei capelli!**

## Formattazione totale ESP32

1. Aprire il terminale di CLion
2. Avviare il comando `idf.py erase_flash`.