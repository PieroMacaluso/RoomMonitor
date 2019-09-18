//
// Created by pieromack on 11/09/19.
//

#ifndef ROOMMONITOR_MAINWINDOW_H
#define ROOMMONITOR_MAINWINDOW_H


#include "ui_main.h"
#include "ui_about.h"
#include "../monitoring/MonitoringServer.h"
#include "windows/elements/callout.h"

class MainWindow: public QMainWindow {
    Ui::MainWindow ui{};
    MonitoringServer s{};
    Callout *m_tooltip = nullptr;
    QChart *chart1;

public:
    MainWindow();

    /**
     * Setup della finestra principale
     */
    void setupConnect();
    /**
     * Funzione per impostare il Grafico numero uno
     */
    //TODO: Creare una funzione simile per l'altro grafico
    void setPlotMacOne();

    void tooltip(bool status, int index, QBarSet * set);
};


#endif //ROOMMONITOR_MAINWINDOW_H
