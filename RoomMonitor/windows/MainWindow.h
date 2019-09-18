//
// Created by pieromack on 11/09/19.
//

#ifndef ROOMMONITOR_MAINWINDOW_H
#define ROOMMONITOR_MAINWINDOW_H


#include "ui_main.h"
#include "ui_about.h"
#include "../monitoring/MonitoringServer.h"
#include "windows/elements/callout.h"
#include <QtCharts>
#include <windows/elements/MonitoringChart.h>

class MainWindow: public QMainWindow {
    Ui::MainWindow ui{};
    MonitoringServer s{};
    QGraphicsSimpleTextItem *m_tooltip = nullptr;
    MonitoringChart *monitoringChart;
    QLineSeries * lineSeries;
    QDateTime startTime{};
            //TODO: remove
    int i_time = 0;

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
    void setupMonitoringPlot();

//    void tooltip(bool status, int index, QBarSet * set);
};


#endif //ROOMMONITOR_MAINWINDOW_H
