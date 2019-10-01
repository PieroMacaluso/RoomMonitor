//
// Created by pieromack on 11/09/19.
//

#ifndef ROOMMONITOR_MAINWINDOW_H
#define ROOMMONITOR_MAINWINDOW_H


#include "ui_main.h"
#include "ui_about.h"
#include "ui_localize.h"
#include "../monitoring/MonitoringServer.h"
#include "windows/common/callout.h"
#include <QtCharts>
#include <windows/plots/monitoring/MonitoringChart.h>
#include <windows/plots/mac/MacChart.h>
#include <windows/classes/LastMac.h>

class MainWindow : public QMainWindow {

    int i_time = 0;

    QThread workerThread;
    /**
     * UI Finestra Principale
     */
    Ui::MainWindow ui{};

    /**
     * UI Dialogo Posizione MAC
     */
    Ui::PositionDialog positionDialog{};

    /**
     * Struttura di monitoraggio. Parte centrale delle procedure di monitoraggio
     */
    MonitoringServer s{};

    QTimer liveGraph{};

    QMap<QString, LastMac> lastMacs{};

public:
    /**
     * Costruttore: si occupa di fare il setup della UI, inizializzare le connect e infine triggerare una funzione per
     * attivare la visualizzazione iniziale, ovvero quella del monitoraggio.
     */
    MainWindow();

    /**
     * Setup delle connect di tutta la MainWindow
     */
    void setupConnect();

    /**
     * Funzione per impostare il grafico del monitoraggio
     */
    void setupMonitoringPlot();

    /**
     * Inizializza la lista delle ultime posizioni dei Mac rilevati
     */
    void initializeLastMacList();

    /**
     * Aggiunta di una ultima posizione MAC alla tabella delle ultime posizioni.
     * @param mac   MAC del dispositivo
     * @param posx  coordinata x posizione
     * @param posy  coordinata y posizione
     */
    void addLastMacPos(const QString &mac, const QDateTime &date, qreal posx, qreal posy);

    /**
     * Inizializza la tabella della situazione dei MAC nel range specificato dall'analisi di lungo periodo
     */
    void initializeMacSituationList();

    /**
     * Aggiunta MAC alla lista della situazione MAC nella statistica di lungo periodo.
     * @param mac   MAC del dispositivo
     * @param posx  coordinata x posizione
     * @param posy  coordinata y posizione
     * @param random    mac randomico o normale?
     */
    void addMacSitua(const QString &mac, int frequency, bool random);

    /**
     * Setup iniziale del grafico andamento presenze nella analisi di lungo periodo
     */
    void setupAnalysisPlot();

    /**
     * Setup iniziale del grafico con le differenti rilevazioni dei MAC
     */
    void setMacPlot();

    /**
     * Setup iniziale del grafico che rappresenta le varie posizioni del MAC selezionato nella relativa finestra di
     * dialogo
     */
    void setupPositionPlot();

    void addLiveData();
    void genLiveData();

    void updateLastMac();

    void dataAnalysis();

    void handleResults(MonitoringChart *chart);

    void updateProgress(qint64 prog);
};


#endif //ROOMMONITOR_MAINWINDOW_H
