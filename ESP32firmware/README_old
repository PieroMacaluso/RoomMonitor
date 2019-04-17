

# OLD VERSION (DEPRECATA)
> Verrà cancellata non appena la nuova versione sarà completa

## Istruzioni per importazione e utilizzo
Per poter sviluppare, testare o implementare nuove funzioni si consiglia di seguire le seguenti istruzioni che sono estrapolate dalla [documentazione ufficiale](http://esp-idf.readthedocs.io/en/latest/get-started/index.html#start-a-project) con le opportune modifiche.

In queste istruzioni si suppone che il progetto sia già stato clonato nella sua interezza sul PC e che sia stata installata correttamente la [**Toolchain**](http://esp-idf.readthedocs.io/en/latest/get-started/index.html#setup-toolchain) per lo sviluppo.


## Importazione del Progetto

* Avviare Eclipse, scegliere *File -> Import...*
* Nella finestra di dialogo, scegliere *"C/C++" -> "Existing Code as Makefile Project"* e cliccare *Next*.
* Nella pagina successiva, inserisci in *"Existing Code Location"* la directory del progetto **ESP32 Firmware**. La cartella dovrà contenere un file denominato *"Makefile"*.
* Nella stessa pagina, sotto *"Toolchain for Indexer Settings"* togliere la spunta da *"Show only available toolchains that support this platform"*.
* Scegliere *"Cygwin GCC"*. E cliccare *"Finish"*.

*Note: nel progetto ci saranno tanti warning: ignoriamoli.*


## Proprietà del Progetto

* Troverete ora il progetto nel Project Explorer di Eclipse. Aprite le Proprietà cliccando con il tasto destro sul progetto.

* Sotto la sezione *"C/C++ Build"*:

  * Togliere la spunta su *"Use default build command"* e andare a sostituire tutta la stringa presente con ``python ${IDF_PATH}/tools/windows/eclipse_make.py``.

* Cliccare su *"Environment"* e sotto *"C/C++ Build"*:

  * Clicca su *"Add..."* e inserisci nome ``BATCH_BUILD`` e valore ``1``.
  * Clicca su "Add..." di nuovo, e inserisci nome ``IDF_PATH``. Il valore dovrà essere il PATH dove ESP-IDF è installato.
    * es. `C:/msys32/home/MyUser/esp/esp-idf`. **NOTA BENE:** usare `/` e non `\`
  * Modificare la variabile d'ambiente *PATH*. Cancellare tutto e inserire:``C:\msys32\usr\bin;C:\msys32\mingw32\bin;C:\msys32\opt\xtensa-esp32-elf\bin`` (Dipende da dove è stato installato **msys32**).

* Clicca su *"C/C++ General"* -> *"Preprocessor Include Paths, Macros, etc."*:

  * Seleziona *"Providers"* tab

     * In *"CDT GCC Built-in Compiler Settings Cygwin"* e su *"Command to get compiler specs"* sostituisci tutto con ``xtensa-esp32-elf-gcc ${FLAGS} -E -P -v -dD "${INPUTS}"``.

     * In *"CDT GCC Build Output Parser"* e quindi su  *"Compiler command pattern"* sostituire tutto con ``xtensa-esp32-elf-((g?cc)|([gc]\+\+)|(clang))``

## Building in Eclipse

* Salvare le impostazioni e uscire dalla schermata delle proprietà.
* Aprire **msys32** e navigare nella cartella contenente il nostro progetto.
  Avviare il comando ``make menuconfig`` per configurare la porta utilizzata.

* Tornate in Eclipse e selezionate *Project -> Build project*

**TIP**: Se avevi già fatto una build fuori da Eclipse, forse dovresti fare *Project -> Clean* prima di fare un altro build in Eclipse.

## Flash da Eclipse

* Click con il tasto destro sul progetto nel Project Explorer.

* Seleziona *Build Targets -> Create...* dal menù contestuale.

* Scrivere *"flash"* come *Target Name*. Lasciare invariato tutto il resto

* Ora si potrà usare *Project -> Build Target -> Build (Shift+F9)* per compilare e flashare il firmware direttamente da Eclipse

Dovremo comunque usare `make menuconfig` per selezionare la porta seriale corretta. 

Stessi step per aggiungere ``bootloader`` and ``partition_table`` target, se necessario.
