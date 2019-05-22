# Progetto ESP32 Firmware
> **[VECCHIA GUIDA (DEPRECATA)](https://github.com/pieromacaluso/PDSproject/blob/master/ESP32firmware/README_old.md)**: verrà eliminata non appena questa guida sarà conclusa.

**IDE Utilizzato**: [CLion](https://www.jetbrains.com/clion/)Open Source

## TOC
1. [Bootstrapping](#bootstrapping)
2. [Apertura Progetto](#Apertura-Progetto)
3. [Build del Progetto](#Build-del-Progetto)
4. [Flash del Progetto](#Build-del-Progetto)
5. [Captive Portal](#Captive-Portal)
6. [Monitor Applicazione](#Monitor-Applicazione)
7. [Quick Fix per tutti i problemi](#Quick-Fix-per-tutti-i-problemi)


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
2. Avviare il comando `idf.py -b 115200 flash`

Il progetto verrà flashato, ora non resta che caricare il nostro Captive Portal e il gioco sarà fatto!

## Captive Portal
### Come creare partizione SPIFFS
> Questa parte è **facoltativa**, affrontarla solo se si vogliono applicare modifiche al captive portal a livello HTML, CSS, JS.

0. Apportare le modifiche desiderate ai file presenti nella cartella `captive_portal`.
1. Aprire il terminale nella root del progetto di VSCode premendo su `Terminal -> New Terminal` nella barra dei menù o usare il terminale precedentemente aperto.
2. Aprire la finestra di terminale nella root del progetto e avviare il comando: 
```bash
python $IDF_PATH/components/spiffs/spiffsgen.py <image_size> <base_dir> <output_file>
```
ovvero
```bash
python $IDF_PATH/components/spiffs/spiffsgen.py 0x100000 captive_portal new_captive_portal.img
```

### Come flashare partizione SPIFFS
1. Aprire il terminale di VSCode premendo su `Terminal -> New Terminal` nella barra dei menù o usare il terminale precedentemente aperto
2. Avviare il seguente comando:
```
python $IDF_PATH/components/esptool_py/esptool/esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 115200 write_flash -z 0x110000 new_captive_portal.img
```

Il captive portal verrà flashato, a questo punto non rimane che usare `make monitor` per verificarne il corretto funzionamento.


## Monitor Applicazione
1. Aprire il terminale di CLion
2. Avviare il comando `idf.py -b 115200 monitor`

## Quick Fix per tutti i problemi
Se riscontrate qualche problema, prima di mettervi le mani nei capelli:
- Avviate da terminale: `idf.py clean` e `idf.py fullclean`.
- Da Clion andate su `File -> Reload CMake project`
- **Mettetevi le mani nei capelli!**
