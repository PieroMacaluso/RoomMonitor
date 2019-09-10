#include <iostream>
#include <QApplication>
#include <QDebug>
#include <QMainWindow>
#include <QtCore/QSettings>
#include "ui_main.h"
#include "ui_settings.h"
#include "monitoring/MonitoringServer.h"

void setupSettings();


int main(int argc, char **argv) {
    QApplication a{argc, argv};
    auto w = new QMainWindow();

    Ui::MainWindow ui;
    Ui::ConfigDialog configDialog;
    ui.setupUi(w);
    w->show();
    MonitoringServer s{ui, configDialog};
    s.setup();

    return a.exec();
}
