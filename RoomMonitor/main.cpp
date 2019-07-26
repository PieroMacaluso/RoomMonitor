#include <iostream>
#include <QApplication>
#include <QDebug>
#include <QMainWindow>
#include "ui_main.h"
#include "MonitoringServer.h"

int main(int argc, char **argv) {
    QApplication a{argc, argv};
    auto w = new QMainWindow();
    Ui::MainWindow ui;
    ui.setupUi(w);
    w->show();
    MonitoringServer s{};
    s.setup(ui);

    return a.exec();
}