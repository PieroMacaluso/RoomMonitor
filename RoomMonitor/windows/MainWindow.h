//
// Created by pieromack on 11/09/19.
//

#ifndef ROOMMONITOR_MAINWINDOW_H
#define ROOMMONITOR_MAINWINDOW_H


#include "ui_main.h"
#include "ui_about.h"
#include "../monitoring/MonitoringServer.h"

class MainWindow: public QMainWindow {
    Ui::MainWindow ui{};
    MonitoringServer s{};

public:
    MainWindow();

    /**
     * Setup della finestra principale
     */
    void setupConnect();

    void setPlotMacOne();
};


#endif //ROOMMONITOR_MAINWINDOW_H
