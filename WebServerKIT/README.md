# WebServerKIT
## Description
This project is our first implementation of a web server useful to load configurations on our ESP32 board without re-flashing the stock firmware.
In order to make the project working you have the instructions provided below.
You can find the configuration of the partitions in `partitions_spiffs.csv`.

## HOW-TO
### Create a SPIFFS partition
***(If you don't want to create and flash your personal partition you can skip these part)***
1. Open the folder `spiffsKIT` and create your personal webpage in the folder `spiffs_image` using HTML, CSS and JS;
2. Now you can create your partition using `mkspiffs` provided in the folder. You can do that with the command `./mkspiffs -c ./spiffs_image -b 4096 -p 256 -s 0x100000 captive_portal.img` (see `mkspiffs --help` for further informations);

**BE CAREFUL**:

- `mkspiffs -v` must have the same parameters of the `sdkconfig` of your application;
- The dimension of the image must be the same of the space available in the `partitions_spiffs.csv`.

### Flash a SPIFFS partition
Use this command:
```
python2 /home/pieromack/esp/esp-idf/components/esptool_py/esptool/esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 115200  write_flash 0x110000 ./captive_portal.img
```
Replacing `/home/pieromack/esp/esp-idf/components/esptool_py/esptool/esptool.py` with your path.

### Flash your software

Use this command in the directory of the program:
```
make flash
```
in the main di