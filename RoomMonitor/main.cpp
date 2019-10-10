#include <iostream>
#include <QApplication>
#include <QDebug>
#include <QMainWindow>
#include <QtCore/QSettings>
#include <QtWidgets/QStyleFactory>
#include "monitoring/MonitoringServer.h"
#include "windows/MainWindow.h"
#include "windows/SettingDialog.h"

int main(int argc, char **argv) {
    QApplication a{argc, argv};
    int ret;
    try {
        SettingDialog::settingCheckUp();
        MainWindow w;
        w.show();
        ret = QApplication::exec();
    } catch (const std::exception & e) {
        // clean up here, e.g. save the session
        // and close all config files.
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE; // exit the application
    }

    return ret;

}
