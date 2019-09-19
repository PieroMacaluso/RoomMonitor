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
    QSettings settings{"VALP", "RoomMonitoring"};

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

    /**
 * veririca se sono presenti mac simili a source oppure no
 * @param source
 * @param initTime
 * @param endTime
 * @return
 */
    bool getHiddenDeviceFor(Packet source,uint32_t initTime,uint32_t endTime){
        //entro 5 minuti, stessa posizione +-0.5, altro da vedere
        uint32_t tolleranzaTimestamp=240;//usata per definire entro quanto la posizione deve essere uguale, 240= 4 minuti
        double tolleranzaX=0.5;     //todo valutare se ha senso impostare le tolleranze da impostazioni grafiche
        double tolleranzaY=0.5;
        double perc;
        bool trovato= false;

        std::deque<Packet> hiddenPackets=getHiddenPackets(initTime,endTime);


        for(int j=0;j<hiddenPackets.size();j++){
            if(hiddenPackets.at(j).getMacPeer()!=source.getMacPeer()){
                double diff=(source.getTimestamp()<hiddenPackets.at(j).getTimestamp()) ? (hiddenPackets.at(j).getTimestamp()-source.getTimestamp()) : (source.getTimestamp()-hiddenPackets.at(j).getTimestamp());
                if(diff<=tolleranzaTimestamp){
                    //mac diverso ad intervallo inferiore di 1 minuto
                    double diffX=(source.getX()<hiddenPackets.at(j).getX()) ? (hiddenPackets.at(j).getX()-source.getX()) : (source.getX()-hiddenPackets.at(j).getX());
                    double diffY=(source.getY()<hiddenPackets.at(j).getY()) ? (hiddenPackets.at(j).getY()-source.getY()) : (source.getY()-hiddenPackets.at(j).getY());
                    if(diffX<=tolleranzaX && diffY<=tolleranzaY){
                        //mac diverso con posizione simile in 4 minuto=> possibile dire che sia lo stesso dispositivo
                        perc=(100-((diffX*100/tolleranzaX) + (diffY*100/tolleranzaY) + (diff*100/tolleranzaTimestamp))*100/(300));
                        std::cout << source.getMacPeer() << " simile ad " << hiddenPackets.at(j).getMacPeer() << " con probabilita' del "<<perc<<"%" << std::endl;
                        //todo decidere cosa fare con tale percentiale
                        trovato= true;
                    }
                }

            }
        }

        return trovato;
    }

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

    int getHiddenDevice(uint32_t initTime,uint32_t endTime){
        bool trovato;
        int numHiddenDevice=0;


        std::deque<Packet> hiddenPackets=getHiddenPackets(initTime,endTime);

        for(int i=0;i<hiddenPackets.size();i++){
            trovato=getHiddenDeviceFor(hiddenPackets.at(i),initTime,endTime);
            if(trovato)
                numHiddenDevice++;
        }

        return numHiddenDevice;
        //todo decidere cosa fare con tale numero
    }

    std::deque<Packet> getHiddenPackets(uint32_t initTime,uint32_t endTime){
        //todo creare la query corretta per ottenere i pacchetti con mac hidden nel periodo specificato
        std::deque<Packet> hiddenPackets;



        return hiddenPackets;
    }

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
