# Progetto ESP32 Firmware
> **[VECCHIA GUIDA (DEPRECATA)](https://github.com/pieromacaluso/PDSproject/blob/master/ESP32firmware/README_old.md)**: verrà eliminata non appena questa guida sarà conclusa.

**IDE Utilizzato**: [Visual Studio Code](https://code.visualstudio.com/) Open Source

## Bootstrapping
**Questa parte è molto importante e necessaria per poter avviare qualsiasi codice sorgente presente nella repository per quanto riguarda ESP32. Attenersi in maniera categorica a tutte le istruzioni riportate in documentazione.**

Per poter sviluppare, testare o implementare nuove funzioni si consiglia di seguire le istruzioni che trovate nella [documentazione ufficiale](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html), in particolare dal punto **1** al punto **4**.

In queste istruzioni si suppone che il progetto sia già stato clonato nella sua interezza sul PC e che sia stata installata correttamente la [**Toolchain**](https://docs.espressif.com/projects/esp-idf/en/latest/get-started-cmake/index.html#installation-step-by-step) per lo sviluppo.

Nota: Nella sezione di Toolchain Setup, della guida sopra citata, si fa riferimento al possibile errore '**Permission issues /dev/ttyUSB0**'. Se il procedimento indicato non dovesse risolvere il problema, usare il comando:
```bash
sudo chmod 777 /dev/ttyUSB0
```

## Apertura Progetto
Aprire la cartella **ESP32Firmware** in VSCode. Il metodo più efficace su Linux è il seguente:
1. Aprire una finestra di terminale
2. Posizionarsi all'interno della cartella `ESP32Firmware`
3. Avviare il comando `code .` 

Altrimenti è sempre possibile farlo aprendo VSCode e premere su `File -> Apri Cartella`.

La cartella `.vscode` nella root del progetto è molto importante. E' quella che permette a  VSCode di rilevare le varie cartelle da cui prendere le interfacce per fornire hint o documentazione del codice.

## Build del Progetto
1. Aprire il terminale di VSCode premendo su `Terminal -> New Terminal` nella barra dei menù.
2. Avviare il comando `make`

Il progetto verrà buildato e verranno mostrati warning ed errori, se trovati. Molto importante è il comando molto elaborato che troverete alla fine dello script: dovrete copiarlo e incollarlo per flashare il firmware.

Il comando sarà simile a questo.
```bash
python2 /home/pieromack/esp/esp-idf/components/esptool_py/esptool/esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 115200 --before default_reset --after hard_reset write_flash -u --flash_mode dio --flash_freq 40m --flash_size detect 0x1000 /home/pieromack/development/esp32/PDSproject/ESP32firmware/build/bootloader/bootloader.bin 0x10000 /home/pieromack/development/esp32/PDSproject/ESP32firmware/build/app-template.bin 0x8000 /home/pieromack/development/esp32/PDSproject/ESP32firmware/build/partitions_spiffs.bin
```
**USARE IL PROPRIO COMANDO, ovviamente questo non funzionerà sul vostro PC!**

Questo contiene tutte le opzioni per flashare le partizioni in maniera corretta, come indicato nel file `partitions_spiffs.csv`.

## Flash del Progetto
1. Aprire il terminale di VSCode premendo su `Terminal -> New Terminal` nella barra dei menù o usare il terminale precedentemente aperto
2. Avviare il comando copiato nella parte finale del punto precedente

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
