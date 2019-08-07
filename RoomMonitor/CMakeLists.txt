cmake_minimum_required(VERSION 3.13)
project(RoomMonitor)
#include_directories(/home/pieromack/Qt/5.13.0/gcc_64/lib/cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Charts REQUIRED)
find_package(Qt5Network REQUIRED)

list(APPEND LIBRARIES
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Charts
        Qt5::Network)
list(REMOVE_DUPLICATES LIBRARIES)

add_executable(RoomMonitor main.cpp MonitoringServer.cpp MonitoringServer.h Packet.cpp Packet.h)
target_link_libraries(RoomMonitor ${LIBRARIES})