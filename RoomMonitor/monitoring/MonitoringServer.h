#ifndef ROOMMONITOR_MONITORINGSERVER_H
#define ROOMMONITOR_MONITORINGSERVER_H

#define _DEBUG 1

#ifdef _DEBUG
#define DEBUG(stuff) std::cout << stuff << std::endl;
#else
#define DEBUG(stuff)
#endif

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <array>
#include "Packet.h"
#include "PositionData.h"
#include "Board.h"
#include <mutex>
#include <QTimer>
#include <map>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QtWidgets/QMessageBox>
#include <QDebug>
#include <QDateTime>
#include <QSettings>
#include <QtCore/QSettings>
#include <QtSql/QSqlRecord>
#include <bitset>
#include "Utility.h"
#include "Circle.h"
#include "../windows/SettingDialog.h"
#include "monitoring/classes/Statistic.h"
#include "monitoring/Board.h"



class MonitoringServer : public QObject {
Q_OBJECT
    /**
     * TCP Server
     */
    QTcpServer server{};

    /**
     * Mappa di <IDSchedina,Numero di fallimenti> per verificarne il corretto funzionamento
     */
    std::map<int, int> board_fail;

    /**
     * Mappa di schedine
     */
    std::map<int, Board> boards;

    /**
     * Lista per controllo ID
     */
    QList<int> check_id;

    /**
     * Buffer di pacchetti second-chance
     */
    std::map<std::string, std::pair<std::deque<Packet>, int>> packets;

    /**
     * Timer per aggregazione
     */
    QTimer timer{};

    /**
     * Impostazioni del software
     */
    QSettings settings{Utility::ORGANIZATION, Utility::APPLICATION};

public:
    /**
     * Costruttore
     */
    MonitoringServer();

    /**
     * Distruttore
     */
    ~MonitoringServer();

    /**
     * Check if the server is running
     * @return true if it is running, false otherwise
     */
    bool isRunning();

    /**
     * Funzione che viene avviata non appena viene premuto il pulsante Start
     * Si occupa di inizializzare il server (127.0.0.1:27015) e di impostare la callback di ogni nuova connessione
     * a this->newConnection
     */
    bool start();

    /**
     * Chiude il server non appena viene premuto il pulsante di stop
     */
    void stop();

    /**
     * Funzione di conversione da RSSI a coordinate partendo da una deque di pacchetti che appartengono allo stesso
     * dispositivo, ma sono state ricevute da schedine differenti. Ritorna una struttura che rappresenta la stima della posizione.
     * @param deque     Deque di pacchetti
     * @return PositionData     posizione stimata del dispositivo
     */
    PositionData fromRssiToXY(const std::deque<Packet>& deque);
    PositionData trilateration(const std::deque<Packet> &deque);
    PositionData trilaterationAverage(const std::deque<Packet> &deque);
    PositionData near(const std::deque<Packet> &deque);



    /**
     * From RSSI and A to metres
     * @param rssi      RSSI value
     * @param A         A constant
     * @return float    distance
     */
    float calculateDistance(signed rssi, int A);

    /**
     * Check if the packet is inside the room, otherwise discard it
     * @param data Position Data
     * @return true if it is inside, false otherwise
     */
    bool is_inside_room(PositionData data);

    /**
     * Check if a mac is hidden/random or not
     * @param basicString
     * @return true if it is random, false otherwise
     */
    static bool isRandomMac(const std::string &basicString);

public slots:

    /**
     * Slot new Connection
     * E' il corpo principale che rappresenta cosa bisogna fare ogni volta che si presenta una nuova connessione
     */
    void newConnection();;

    /**
     * Slot che avvia l'aggregazione
     */
    void aggregate();

signals:

    /**
     * Signal che segnala l'avvio del Server TCP
     */
    void started();

    /**
     * Signal che segnala la conclusione della fase di aggregazione
     */
    void aggregated();

    /**
     * Signal che segnala la chiusura del server TCP
     */
    void stopped();
};

#endif //ROOMMONITOR_MONITORINGSERVER_H
