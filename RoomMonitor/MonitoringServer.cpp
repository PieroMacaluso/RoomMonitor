//
// Created by pieromack on 26/07/19.
//

#include "MonitoringServer.h"

std::deque<Packet> MonitoringServer::string2packet(const std::vector<std::string>& p) {
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
void MonitoringServer::split(const std::string &str, Container &cont, char delim){
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim)) {
        cont.push_back(token);
    }
}
/**
 * Questa funzione prende in ingresso una coda di pacchetti con lo stesso FCS e provenienti da schede differenti.
 * L'obiettivo è restituire
 * @param deque
 * @return
 */
PositionData MonitoringServer::fromRssiToXY(std::deque<Packet> deque) {
    // TODO: Calcolo della posizione da RSSI
    return PositionData(0.5, 0.5);
}

void MonitoringServer::connectDB() {
    if (!nDatabase.open()) {
        qDebug() << nDatabase.lastError();
        return;
    }
}

void MonitoringServer::disconnectDB() {
    nDatabase.close();
}

void MonitoringServer::serverStart() {
    server = new QTcpServer(this);


    if (!server->listen(QHostAddress::Any, 27015)) {
        qDebug() << "Server Did not start";
    } else {
        QObject::connect(server, &QTcpServer::newConnection, this, &MonitoringServer::newConnection);
        qDebug() << "Server Started on port:" << server->serverPort();
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &MonitoringServer::aggregate);
        timer->start(30000);
    }
}

void MonitoringServer::serverStop() {
    server->close();
    timer->stop();
    delete timer;
    timer = nullptr;
}

void MonitoringServer::setup(Ui::MainWindow &ui) {
    ui.stopButton->setDisabled(true);

    // Click Start Button

    QObject::connect(ui.startButton, &QPushButton::clicked, [&]() {
        try {
            int n = ui.nSchedine->text().toInt();
            if (n > 0) {
                this->nSchedine = n;
                this->started(ui.nSchedine->text().toInt());
                ui.startButton->setDisabled(true);
                ui.stopButton->setDisabled(false);
            }
        } catch (std::exception &e) {
            // Does not started signal
            std::cout << "Non è stato possibile avviare il server." << std::endl;
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
void
MonitoringServer::splitString(const std::string &str, Container &cont, std::string &startDelim, std::string &stopDelim) {
    unsigned first = 0;
    unsigned end = 0;
    while ((first = str.find(startDelim, first)) < str.size() && (end = str.find(stopDelim, first)) < str.size()) {
        std::string val = str.substr(first + startDelim.size() + 1, end - first - startDelim.size() - 2);
        cont.push_back(val);
        first = end + stopDelim.size();
    }
}
