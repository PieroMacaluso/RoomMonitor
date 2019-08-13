//
// Created by pieromack on 26/07/19.
//

#include "MonitoringServer.h"
#include "Circle.h"

MonitoringServer::MonitoringServer() {
    timer = nullptr;
    nDatabase = QSqlDatabase::addDatabase("QMYSQL");
    nDatabase.setHostName("localhost");
    nDatabase.setDatabaseName("data");
    nDatabase.setPort(3306);
    nDatabase.setUserName("root");
    nDatabase.setPassword("NewRoot12Kz");

    // TODO: Setup schedine da implementare
    // Ipotizziamo stanza 5.6mx2.3m con schedine in diagonale
    Board b0{0, 0, 0};
    Board b1{1, 3.2, 5.6};
    boards.insert(std::make_pair(0, b0));
    boards.insert(std::make_pair(1, b1));
}

MonitoringServer::~MonitoringServer() {
    if (timer != nullptr) {
        delete timer;
    }
}


std::deque<Packet> MonitoringServer::string2packet(const std::vector<std::string> &p) {
    std::deque<Packet> deque;
    std::string ssid;

    for (const std::string &s:p) {
        std::vector<std::string> values;
        split(s, values, ',');

        if (values.size() == 7)        //alcuni pacchetti contengono l'ssid e altri no
            //no ssid
            ssid = "Nan";
        else
            ssid = values[7];

        Packet packet(std::stoi(values[0]), values[3], std::stoi(values[4]), values[5], std::stoi(values[6]), ssid);
        deque.push_back(packet);
    }

    return deque;
}

template<class Container>
void MonitoringServer::split(const std::string &str, Container &cont, char delim) {
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim)) {
        cont.push_back(token);
    }
}

/**
 * Questa funzione prende in ingresso una coda di pacchetti con lo stesso FCS(hash) e provenienti da schede differenti.
 * L'obiettivo è restituire la posizione stimata. Restituisce PositionData(-1,-1) se dato non va bene.
 * @param deque
 * @return
 */
PositionData MonitoringServer::fromRssiToXY(std::deque<Packet> deque) {
    // Deque di cerchi
    std::deque<Circle> circles{};
    // Deque che ci serve per media pesata
    std::deque<std::pair<PositionData, double>> pointW;

    // Da pacchetti a Cerchi di centro schedina e raggio RSSI->metri.
    for (auto packet: deque) {
        auto b = boards.find(packet.getIdSchedina());
        if (b == boards.end()) return PositionData(-1, -1);
        double dist = calculateDistance(packet.getRssi());
        Circle res{dist, b->second.getCoord().x(), b->second.getCoord().y()};
        circles.push_back(res);
    }

    // TODO: controllare che tutte le circonferenze si intersechino
    // Combinazioni  e controllo, se anche una non è soddisfatta allora ingrandisco tutte le circonferenze dello spazio che manca.
    for (int i = 0; i < circles.size() - 1; i++) {
        for (int j = i + 1; j < circles.size(); j++) {
            // Punti di intersezione
            Point2d intPoint1, intPoint2;
            // Calcolare intersezione

            size_t i_points = circles[i].intersect(circles[j], intPoint1, intPoint2);
            if (i_points <= 1)// INTERSEZ SOLO 1 PUNTO o 0
            {
                std::deque<Circle>::iterator it = circles.begin();// RISCRIVO/SOVRASCRIVO STESSI VALORI CON VALORI CORRETTI
                for (auto packet: deque) {
                    auto b = boards.find(packet.getIdSchedina());
                    if (b == boards.end()) return PositionData(-1, -1);
                    double dist = calculateDistance(packet.getRssi());
                    int incr;
                    incr= sqrt(pow( b->second.getCoord().x(), 2)+pow( b->second.getCoord().y(), 2));// calcolo di quanto deve essere l'incremento considerando come centro una circonf
                    //dist=dist+(dist- b->second.getCoord().x()-b->second.getCoord().y())/2+0.5;
                    // AUMENTO DEL RAGGIO. NON SO SE L'AUMENTO E' CORRETTO???
                    dist=dist+(dist- incr)+0.5;
                    Circle res{dist, b->second.getCoord().x(), b->second.getCoord().y()};
                    //SOVRASCRIVO I VALORI PRECEDENTI CON RAGGIO CORRETTO
                    it = circles.insert (it,res);
                    ++it;
                    //circles.push_back(res);

                }



            }

        }
    }

    // Combinazioni  e controllo, se anche una non è soddisfatta allora ingrandisco tutte le circonferenze dello spazio che manca.

    // Doppio ciclo per combinazioni
    for (int i = 0; i < circles.size() - 1; i++) {
        for (int j = i + 1; j < circles.size(); j++) {
            // Punti di intersezione
            Point2d intPoint1, intPoint2;
            // Calcolare intersezione
            // TODO: rivedere bene funzione intersect
            size_t i_points = circles[i].intersect(circles[j], intPoint1, intPoint2);
            if (i_points == 0) return PositionData{-1, -1};
            // Controllo se questi punti sono contenuti nelle altre circonferenze.
            // Ipotesi verificata a meno che non si trovi una condizione falsa.
            bool ok1 = true;
            bool ok2 = i_points > 1;
            for (int k = 0; k < circles.size(); k++) {
                // Non controllare su circonferenze oggetto di intersezione
                if (k == i || k == j) continue;

                if (!circles[k].containsPoint(intPoint1)) {
                    ok1 = false;
                }
                if (i_points > 1) {
                    if (!circles[k].containsPoint(intPoint2)) {
                        ok2 = false;
                    }
                }
            }

            if (ok1) {
                // CALCOLO CON MEDIA PESATA
                PositionData mean_res{};
                mean_res.addPacket(intPoint1.x(), intPoint1.y());
                pointW.emplace_back(mean_res, circles[i].getR() + circles[j].getR());
            }
            if (ok2) {
                // CALCOLO CON MEDIA PESATA
                PositionData mean_res{};
                mean_res.addPacket(intPoint2.x(), intPoint2.y());
                pointW.emplace_back(mean_res, circles[i].getR() + circles[j].getR());
            }
        }
    }

    // CALCOLO FINALE MEDIA PESATA
    double num_x = 0;
    double num_y = 0;
    double den = 0;
    std::for_each(pointW.begin(), pointW.end(),
                  [&](std::pair<PositionData, double> pair) {
                      num_x += pair.first.getX() / pair.second;
                      num_y += pair.first.getY() / pair.second;
                      den += 1 / pair.second;
                  });

    PositionData result{};
    result.addPacket(num_x / den, num_y / den);

    // TODO: (forse fatto, ma da controllare)valido se x e y risultanti sono nella stanza, altrimenti no!
    if (!is_inside_room(result)) return PositionData{-1, -1};
    return result;
}

/**
 * @brief      Conversione molto grossolana da RSSI a Metri
 *
 * @param[in]  rssi  RSSI
 *
 * @return     float in metri
 */
float MonitoringServer::calculateDistance(signed rssi) {
    // n: Costante di propagazione del segnale. Costante 2 in ambiente aperto.
    // TODO: vedere se applicabile a stanza
    const float cost = 2;
    // A: potenza del segnale ricevuto in dBm ad un metro
    // TODO: da ricercare sperimentalmente
    const float A = -59;

    return pow(10, (A - rssi) / (10 * cost));

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
MonitoringServer::splitString(const std::string &str, Container &cont, std::string &startDelim,
                              std::string &stopDelim) {
    unsigned first = 0;
    unsigned end = 0;
    while ((first = str.find(startDelim, first)) < str.size() && (end = str.find(stopDelim, first)) < str.size()) {
        std::string val = str.substr(first + startDelim.size() + 1, end - first - startDelim.size() - 2);
        cont.push_back(val);
        first = end + stopDelim.size();
    }
}

bool MonitoringServer::is_inside_room(PositionData data) {
    // TODO: margine?
    return data.getX() >= 0 && data.getY() >= 0 && data.getX() <= room.getMax().x() && data.getY() <= room.getMax().y();
}

