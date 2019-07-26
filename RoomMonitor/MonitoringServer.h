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

class Packet{
public:
    std::string content;
    int id;
    Packet(int id, const std::string& content): id(id), content(content){}
};

class MonitoringServer : public QObject {
Q_OBJECT
    QTcpServer* server;
    // TODO: nSchedine è ancora inutile, sono da implementare i thread
    int nSchedine;
    bool running;

public:
    MonitoringServer(){

    }
    /**
     * Funzione che viene avviata non appena viene premuto il pulsante Start
     * Si occupa di inizializzare il server (127.0.0.1:1234) e di impostare la callback di ogni nuova connessione
     * a this->newConnection
     */
    void serverStart(){
        server = new QTcpServer(this);
        QObject::connect(server, &QTcpServer::newConnection, this, &MonitoringServer::newConnection);

        if (!server->listen(QHostAddress::Any, 1234)){
            qDebug() << "Server Did not start";
        } else {
            qDebug() << "Server Started";
        }
    }

    /**
     * Chiude il server non appena viene premuto il pulsante di stop
     */
    void serverStop(){
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

        QObject::connect(this, &MonitoringServer::started, [&](){this->serverStart();});

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

    template <class Container>
    void split(const std::string& str, Container& cont, char delim = ';')
    {
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, delim)) {
            cont.push_back(token);
        }
    }


public slots:
    /**
     * Slot new Connection
     * E' il corpo principale che rappresenta cosa bisogna fare ogni volta che si presenta una nuova connessione
     */
    void newConnection(){
        QTcpSocket * socket = server->nextPendingConnection();
        int  numReadTotal;
        std::vector<Packet> data;
        while (socket->waitForReadyRead(10000)){
            QByteArray a = socket->readLine();
            if (!a.isEmpty()) {
                std::string packet = a.toStdString();
                // Eliminazione \n
                packet.erase(packet.find('\n'));
                // Splitting stringa
                std::vector<std::string> vector;
                MonitoringServer::split(packet, vector, ';');
                // Inserimento in struttura che verrò stampata appena scatterà in timeout
                Packet p{std::stoi(vector[0]), vector[1]};
                data.push_back(p);
            }
        }

        for (auto &el: data){
            std::cout << el.id << " " << el.content << std::endl;
        }
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
