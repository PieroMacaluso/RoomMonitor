//
// Created by pieromack on 26/07/19.
//

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
#include "Room.h"
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


class MonitoringServer : public QObject {
Q_OBJECT
    QTcpServer server{};
    std::map<int, Board> boards;
    bool running;
    std::deque<std::pair<Packet, int>> packets;
    std::mutex m;
    QTimer timer{};
    QSqlDatabase nDatabase;
    QSettings settings{};

public:
    MonitoringServer();

    ~MonitoringServer();

    /**
     * Connessione al database
     */
    void connectDB();

    /**
     * Disconnessione dal database
     */
    void disconnectDB();

    bool isRunning();

    /**
     * Funzione utilizzata per convertire il vettore di stringhe ricevute dalla esp in oggetti packet
     * @param packets
     * @return
     */
    std::deque<Packet> string2packet(const std::vector<std::string> &packets);

    template<class Container>
    void split(const std::string &str, Container &cont, char delim);

    /**
     * Funzione che viene avviata non appena viene premuto il pulsante Start
     * Si occupa di inizializzare il server (127.0.0.1:27015) e di impostare la callback di ogni nuova connessione
     * a this->newConnection
     */
    void start();

    /**
     * Chiude il server non appena viene premuto il pulsante di stop
     */
    void stop();

    /**
     * Split fatto con stringhe per evitare caratteri casuali/involuti inviati dalla schedina
     * @tparam Container
     * @param str
     * @param cont
     * @param startDelim
     * @param stopDelim
     */
    template<class Container>
    void splitString(const std::string &str, Container &cont, std::string &startDelim, std::string &stopDelim);

    PositionData fromRssiToXY(std::deque<Packet> deque);

    float calculateDistance(signed rssi);

    bool is_inside_room(PositionData data);


public slots:

    /**
     * Slot new Connection
     * E' il corpo principale che rappresenta cosa bisogna fare ogni volta che si presenta una nuova connessione
     */
    void newConnection();;


    void aggregate();


signals:

    /**
     * Signal che segnala l'avvio del Server TCP
     * @param nSchedine numero delle schedine a disposizione
     */
    void started();

    /**
     * Signal che segnala la chiusura del server TCP
     */
    void stopped();
};


#endif //ROOMMONITOR_MONITORINGSERVER_H
