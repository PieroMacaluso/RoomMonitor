//
// Created by pieromack on 26/07/19.
//

#ifndef ROOMMONITOR_MONITORINGSERVER_H
#define ROOMMONITOR_MONITORINGSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <ui_main.h>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <array>

class Packet {
public:
    std::string mac;
    int id_schedina;

    Packet(std::string mac, int id_schedina) : mac(mac), id_schedina(id_schedina) {}
};

class MonitoringServer : public QObject {
Q_OBJECT
    QTcpServer *server;
    // TODO: nSchedine è ancora inutile, sono da implementare i thread
    int nSchedine;
    bool running;

public:
    MonitoringServer() {

    }

    /**
     * Funzione che viene avviata non appena viene premuto il pulsante Start
     * Si occupa di inizializzare il server (127.0.0.1:1234) e di impostare la callback di ogni nuova connessione
     * a this->newConnection
     */
    void serverStart() {
        server = new QTcpServer(this);
        QObject::connect(server, &QTcpServer::newConnection, this, &MonitoringServer::newConnection);

        if (!server->listen(QHostAddress::Any, 27015)) {
            qDebug() << "Server Did not start";
        } else {
            qDebug() << "Server Started on port:" << server->serverPort();
        }
    }

    /**
     * Chiude il server non appena viene premuto il pulsante di stop
     */
    void serverStop() {
        server->close();
    }

    /**
     * Setup della finestra del Server
     * @param ui Ui finestra principale da cui andare a prendere i vari file.
     */
    void setup(Ui::MainWindow &ui) {
        ui.stopButton->setDisabled(true);

        // Click Start Button

        QObject::connect(ui.startButton, &QPushButton::clicked, [&]() {
            try {
                this->nSchedine = ui.nSchedine->text().toInt();
                this->started(ui.nSchedine->text().toInt());
                ui.startButton->setDisabled(true);
                ui.stopButton->setDisabled(false);
            } catch (std::exception &e) {
                // Does not started signal
                std::cout << "Ecc" << std::endl;
                return;
            }
            this->running = true;

        });

        // Conseguenze Click Start Button

        QObject::connect(this, &MonitoringServer::started, [&]() { this->serverStart(); });

        // Click Stop Button

        QObject::connect(ui.stopButton, &QPushButton::clicked, [&]() {
            this->stopped();
            ui.startButton->setDisabled(false);
            ui.stopButton->setDisabled(true);
            this->running = false;
            this->serverStop();


        });

        // Conseguenze Click Start Button

        QObject::connect(this, &MonitoringServer::stopped, [&]() {
            std::cout << "Stopped" << std::endl;
        });

    }

    template<class Container>
    void split(const std::string &str, Container &cont, char delim) {
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, delim)) {
            cont.push_back(token);
        }
    }

    /**Split fatto con stringhe per evitare caratteri casuali/involuti inviati dalla schedina
     * */
    template<class Container>
    void splitString(const std::string &str, Container &cont, std::string &startDelim, std::string &stopDelim) {
        unsigned first=0;
        unsigned end=0;
        while ((first = str.find(startDelim,first))<str.size()&& (end=str.find(stopDelim,first))<str.size()) {
            std::string val = str.substr(first+startDelim.size()+1,end-first-startDelim.size()-2);
            cont.push_back(val);
            first=end+stopDelim.size();
        }
    }

public slots:

    /**
     * Slot new Connection
     * E' il corpo principale che rappresenta cosa bisogna fare ogni volta che si presenta una nuova connessione
     */
    void newConnection() {
        std::string startDelim("init");
        std::string stopDelim("end");
        QTcpSocket *socket = server->nextPendingConnection();
         int numReadTotal;
        std::vector<std::string> pacchetti;
        while (socket->waitForReadyRead(10000)) {
            QByteArray a = socket->readLine();
            if (!a.isEmpty()) {
                std::string packet = a.toStdString();
                //divisione singoli pacchetti
                MonitoringServer::splitString(packet,pacchetti,startDelim,stopDelim);
                for(std::string s:pacchetti)
                    std::cout << s<< std::endl;
                // Splitting stringa
                std::vector<std::string> vector;

            }
        }

        //todo controllare se va bene un conteiner pacchetti per ogni newConnection (stesso anche in caso la schedina usi più pacchetti tcp per inviare l'intero elenco)
        socket->flush();
        socket->close();
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


#endif //ROOMMONITOR_MONITORINGSERVER_H
