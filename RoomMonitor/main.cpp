#include <iostream>
#include <QApplication>
#include <QDebug>
#include <QMainWindow>
#include <QtCore/QSettings>
#include "ui_main.h"
#include "ui_settings.h"
#include "monitoring/MonitoringServer.h"
#include "windows/MainWindow.h"
#include "windows/SettingDialog.h"

void setupSettings();


int main(int argc, char **argv) {
    QApplication a{argc, argv};
    SettingDialog::settingCheckUp();
    MainWindow w;
    w.show();

    return a.exec();
}
