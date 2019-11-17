//
// Created by pieromack on 11/09/19.
//

#ifndef ROOMMONITOR_MAINWINDOW_H
#define ROOMMONITOR_MAINWINDOW_H


#include "ui_main.h"
#include "ui_about.h"
#include "ui_random.h"
#include "ui_localize.h"
#include "../monitoring/MonitoringServer.h"
#include "windows/common/callout.h"
#include <QtCharts>
#include <windows/plots/monitoring/MonitoringChart.h>
#include <windows/plots/mac/MacChart.h>
#include <windows/classes/LastMac.h>
#include <QtGlobal>

class MainWindow : public QMainWindow {

    int i_time = 0;

    /**
     * Worker thread utilizzato per analisi pesanti. Queste vengono eseguite in un thread separato per evitare di
     * bloccare il thread principale mentre si occupa di renderizzare la parte grafica. Quest'ultimo comunica con il
     * thread principale attraverso
     */
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
     * UI Dialogo Random MAC
     */
    Ui::RandomDialog randomDialog{};

    /**
     * Struttura di monitoraggio. Parte centrale delle procedure di monitoraggio
     */
    MonitoringServer s{};

    /**
     * Timer utilizzato per poter triggerare la query che permette di riempire il grafico con
     * le presenze negli ultimi 5 minuti
     */
    QTimer liveGraph{};

    /**
     * Mappa che viene pulita e riempita ogni volta che è vengono fatte le query corrispondenti alle
     * posizioni degli ultimi MAC
     */
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
    void setupPositionPlot(QString mac);

    /**
     * Funzione triggerata dal timer `liveGraph` ogni 5 minuti. Conteggio MAC nello slot degli ultimi 5 minuti e in quello
     * dei precedenti ultimi 5 minuti. Serve solo a riempire il grafico delle presenze in monitoraggio.
     */
    void addLiveData();

    /**
     * Funzione triggerata dal segnale `MonitoringServer::aggregated` che viene emesso ogni volta che l'aggregazione
     * viene effettuata. Si preoccupa di andare a calcolare le ultime posizioni dei MAC rilevati negli ultimi 5 minuti
     * per riempire il grafico delle posizioni LIVE e preparare i dati per riempire le tabelle degli ultimi MAC.
     */
    void genLiveData();

    /**
     * Funzione da chiamare per riempire la tabella degli ultimi MAC con i dati contenuti nella variabile
     * globale `lastMacs`
     */
    void updateLastMac();

    /**
     * Funzione chiamata dal click sul pulsante Cerca in modalità di Analisi. Si occupa di chiamare il thread che andrà
     * ad effettuare le query necessarie per il calcolo e il popolamento dei grafici.
     */
    void dataAnalysis();

    /**
     * Funzione richiamata dall'emissione del segnale `AnalysisWorker::resultReady` una volta che il thread ha terminato
     * le operazioni di recupero dei dati con le query.
     * @param chart puntatore al grafico che deve essere inserito al posto del precedente nella schermata principale.
     */
    void handleResults(MonitoringChart *chart);

    /**
     * Funzione che permette di all'utente di monitorare la percentuale di completamento delle operazioni eseguite dal
     * thread. Questa funzione viene richiamata all'emissione del segnale `AnalysisWorker::updateProgress`
     * @param prog
     */
    void updateProgress(qint64 prog);

    /**
     * Funzione richiamata dall'emissione del segnale `AnalysisWorker::macPlotReady` una volta che il thread ha terminato
     * le operazioni di recupero dei dati con le query per le posizioni degli ultimi MAC.
     * @param chart puntatore al grafico che deve essere inserito al posto del precedente nella schermata principale.
     */
    /**
     * Funzione richiamata dall'emissione del segnale `AnalysisWorker::macPlotReady` una volta che il thread ha terminato
     * le operazioni di recupero dei dati con le query per le posizioni degli ultimi MAC.
     * @param mac QStringList dei Mac
     * @param frequency QStringList delle frequenze dei mac
     * @param macPlot puntatore al grafico MAC da popolare
     */
    void macPlotReady(QStringList mac, QStringList frequency, MacChart *macPlot);

    /**
     * Funzione chiamata dall'emissione del segnale `AnalysisWorker::finished` per concludere il thread senza problemi
     */
    void finishedAnalysisThread();

    /**
     * Funzione da chiamare per popolare il grafico delle posizioni LIVE con i dati presenti
     */
    void setupLivePlot();

    /**
     * Funzione da chiamare per popolare il grafico delle posizioni con i dati presenti in monitoraggio
     */
    void setupMapPlot();

    void stopServer();

    QList<Statistic> getHiddenMacFor(QString mac, QDateTime initTime, QDateTime endTime);

    static std::deque<Packet> getHiddenPackets(QDateTime initTime, QDateTime endTime, QString &mac);

    std::list<Packet> getAllPacketsOfMac(const QString &mac, QDateTime initTime, QDateTime endTime);
};


#endif //ROOMMONITOR_MAINWINDOW_H
