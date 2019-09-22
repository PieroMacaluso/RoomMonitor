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
#include <windows/elements/Plot1.h>

class MainWindow : public QMainWindow {
    Ui::MainWindow ui{};
    MonitoringServer s{};
    Plot1 *plot1{};
    MonitoringChart *monitoringChart{};
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
    void initializeLastMacList();

    void addLastMacPos(const QString &mac, qreal posx, qreal posy);

    void initializeMacSituationList();

    void addMacSitua(const QString &mac, qreal posx, qreal posy, bool random);

    void setupAnalysisPlot();


};


#endif //ROOMMONITOR_MAINWINDOW_H
