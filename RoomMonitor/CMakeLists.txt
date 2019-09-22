cmake_minimum_required(VERSION 3.13)
project(RoomMonitor)
#include_directories(/home/pieromack/Qt/5.13.0/gcc_64/lib/cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#if(CMAKE_VERSION VERSION_LESS "3.7.0")
#    set(CMAKE_INCLUDE_CURRENT_DIR ON)
#endif()

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Charts REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5PrintSupport REQUIRED)

find_package(Qt5Sql REQUIRED)

list(APPEND LIBRARIES
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Charts
        Qt5::Sql
        Qt5::Network
        Qt5::PrintSupport)
QT5_WRAP_UI(QT_UIS_HDRS
        mainwindow.ui)

# Tell CMake to create the helloworld executable
#add_executable(helloworld qcustomplot.cpp main.cpp mainwindow.cpp ${QT_UIS_HDRS})
#list(REMOVE_DUPLICATES LIBRARIES)

add_executable(RoomMonitor main.cpp monitoring/MonitoringServer.cpp monitoring/MonitoringServer.h monitoring/Packet.cpp monitoring/Packet.h monitoring/PositionData.cpp monitoring/PositionData.h monitoring/Board.cpp monitoring/Board.h monitoring/Circle.cpp monitoring/Circle.h monitoring/Point2d.cpp monitoring/Point2d.h monitoring/Room.h monitoring/Room.cpp windows/SettingDialog.cpp windows/SettingDialog.h windows/MainWindow.cpp windows/MainWindow.h windows/InformationWindow.cpp windows/InformationWindow.h windows/elements/MacChart.h windows/elements/MacChart.cpp windows/elements/callout.h windows/elements/callout.cpp windows/elements/MonitoringPlotView.cpp windows/elements/MonitoringPlotView.h windows/elements/MonitoringChart.cpp windows/elements/MonitoringChart.h res.qrc windows/elements/MacPlotView.cpp windows/elements/MacPlotView.h windows/classes/MacOccurrence.cpp windows/classes/MacOccurrence.h)
target_link_libraries(RoomMonitor ${LIBRARIES})