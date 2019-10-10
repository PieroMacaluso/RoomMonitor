//
// Created by angel on 03/08/2019.
//

#include <sstream>
#include "Ricezione.h"

Ricezione::Ricezione(std::shared_ptr<DequeThreadSafe<Packet>> pacchetti): pacchettiRicevuti(pacchetti) {
    ricezioneStart();
}
Ricezione::~Ricezione() {
    ricezioneStop();
}

/**
 * Funzione utilizzata per convertire il vettore di stringhe ricevute dalla esp in oggetti packet
 * @param packets
 * @return
 */
std::deque<Packet> Ricezione::string2packet(const std::vector<std::string>& p) {
    std::deque<Packet> deque;
    std::string ssid;

    for(const std::string& s:p){
        std::vector<std::string> values;
        split(s,values,',');

        if(values.size()==7)        //alcuni pacchetti contengono l'ssid e altri no
            //no ssid
            ssid="Nan";
        else
            ssid=values[7];

        Packet packet(std::stoi(values[0]),values[3],std::stoi(values[4]),values[5],std::stoi(values[6]),ssid);
        deque.push_back(packet);
    }

    return deque;
}

template<class Container>
void Ricezione::split(const std::string &str, Container &cont, char delim){
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim)) {
        cont.push_back(token);
    }
}

    /**
     * Si occupa di inizializzare il server (127.0.0.1:1234) e di impostare la callback di ogni nuova connessione
     * a this->newConnection
     */
void Ricezione::ricezioneStart() {
    serverRicezione = new QTcpServer(this);
    QObject::connect(serverRicezione, &QTcpServer::newConnection, this, &Ricezione::newConnection);


    if (!serverRicezione->listen(QHostAddress::Any, 27015)) {
        qDebug() << "Server Did not start";
    } else {
        qDebug() << "Server Started on port:" << serverRicezione->serverPort();
    }
}
    /**
     * Chiude il server non appena viene premuto il pulsante di stop
     */
void Ricezione::ricezioneStop() {
    serverRicezione->close();
}




