# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)
set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_common)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(roommonitor-firmware)

spiffs_create_partition_image(storage captive_portal FLASH_IN_PROJECT)
#execute_process(COMMAND python $ENV{IDF_PATH}/components/spiffs/spiffsgen.py 0x100000 captive_portal new_captives_portal.img)
