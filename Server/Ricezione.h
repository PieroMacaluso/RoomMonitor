//
// Created by angel on 03/08/2019.
//

#ifndef SERVER_RICEZIONE_H
#define SERVER_RICEZIONE_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <iostream>
#include "Packet.h"
#include "DequeThreadSafe.h"

class Ricezione : public QObject {
Q_OBJECT
    QTcpServer *serverRicezione;
    std::shared_ptr<DequeThreadSafe<Packet>> pacchettiRicevuti;

    std::deque<Packet> string2packet(const std::vector<std::string>& packets);
    template<class Container>
    void split(const std::string &str, Container &cont, char delim);


public:
    Ricezione(std::shared_ptr<DequeThreadSafe<Packet>> paccheti);
    ~Ricezione();
    void ricezioneStart();
    void ricezioneStop();


public slots:

    /**
     * Slot new Connection
     * E' il corpo principale che rappresenta cosa bisogna fare ogni volta che si presenta una nuova connessione
     */
    void newConnection() {
        std::cout << "New Connection started" << std::endl;
        std::string startDelim("init");
        std::string stopDelim("end");
        QTcpSocket *socket = serverRicezione->nextPendingConnection();
        std::vector<std::string> pacchetti;
        std::deque<Packet> packetsConn;
        std::string allData{};

        // TODO: finetuning di questo parametro, più è piccolo, meglio è!
        while (socket->waitForReadyRead(500)) {
            // Concatenazione stringhe ricevute in un'unica stringa
            QByteArray a = socket->readAll();
            if (!a.isEmpty()) {
                std::string packet = a.toStdString();
                allData += packet;
            }
        }

        // Creazione pacchettini
        if (!allData.empty()) {
            // Divisione singoli pacchetti
            Ricezione::split(allData, pacchetti, ';');
            // Conversione in oggetti Packet
            packetsConn = string2packet(pacchetti);

            //Inserimento in struttura dati threadSafe
            for(Packet p:packetsConn)                   //todo verificare se possibile farlo in altre scansioni senza eseguirne troppe
                pacchettiRicevuti->push_back(p);

            /** INIZIO ESECUZIONE THREAD-SAFE */

            /** FINE ESECUZIONE THREAD-SAFE */
        }


        //todo controllare se va bene un conteiner pacchetti per ogni newConnection (stesso anche in caso la schedina usi più pacchetti tcp per inviare l'intero elenco)
        socket->flush();
        socket->close();
        std::cout << "Connection closed" << std::endl;
        delete socket;

    };


signals:

    /**
     * Signal che segnala l'avvio del Server TCP
     * @param nSchedine numero delle schedine a disposizione
     */
    void started(int nSchedine);

    /**
     * Signal che segnala la chiusura del server TCP
     */
    void stopped();


};


#endif //SERVER_RICEZIONE_H
