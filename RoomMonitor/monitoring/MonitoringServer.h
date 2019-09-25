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
    std::map<std::string, std::pair<std::deque<Packet>, int>> packets;
//    std::deque<std::pair<Packet, int>> packets;
    QTimer timer{};
    QSettings settings{"VALP", "RoomMonitoring"};

public:
    MonitoringServer();

    ~MonitoringServer();

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

    PositionData fromRssiToXY(const std::deque<Packet>& deque);

    float calculateDistance(signed rssi);

    bool is_inside_room(PositionData data);

    bool getHiddenDeviceFor(Packet source, uint32_t initTime, uint32_t endTime, std::deque<Packet> &hiddenPackets);
    bool getHiddenMacFor(QString mac, uint32_t initTime, uint32_t endTime);

/**
 * Restituisce una stima del numero di dispositivi con mac nascosto nell'intervallo di tempo passato.
 * Separato su due funzioni per avere anche solo la ricerca per un singolo mac.
 * @param initTime
 * @param endTime
 *
 * initTime e endTime corrispondono al periodo di osservazione (esempio alcune ore)
 * tolleranzaTimestamp corrisponde al tempo in cui la posizione deve essere simile per dire che il mac è uguale ad un altro (esempio 1 minuto)
 *
 * @return
 */

    int getHiddenDevice(uint32_t initTime, uint32_t endTime);

/**
 * Funzione che recupera dal db tutti i pacchetti con mac hidden nel periodo specificato
 * @param initTime
 * @param endTime
 * @return
 */
    std::deque<Packet> getHiddenPackets(uint32_t initTime, uint32_t endTime);

    /**
 * Funzione utilizzata per ottenere il pacchetto con il maggior timestamp di un determinato mac
 * @param mac
 * @param initTime
 * @param endTime
 * @return
 */
    Packet getLastPacketWithMac(QString mac, uint32_t initTime, uint32_t endTime);

    std::list<PositionData> getAllPositionOfMac(QString mac, uint32_t initTime, uint32_t endTime);

    std::list<uint32_t> getAllTimeStampOfMac(QString mac, uint32_t initTime, uint32_t endTime);

    double checkTimeStamp(uint32_t timestamp, std::list<uint32_t> list);

    double checkPos(PositionData data, std::list<PositionData> list);


public slots:

    /**
     * Slot new Connection
     * E' il corpo principale che rappresenta cosa bisogna fare ogni volta che si presenta una nuova connessione
     */
    void newConnection();;


    void aggregate();

    static bool isRandomMac(const std::string &basicString);


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


    std::list<Packet> getAllPacketsOfMac(QString qString, uint32_t time, uint32_t endTime);
};


#endif //ROOMMONITOR_MONITORINGSERVER_H
