#include "MonitoringServer.h"

MonitoringServer::MonitoringServer() {
}

MonitoringServer::~MonitoringServer() {
}

void recalculateCircles(std::deque<Circle> &circs, double delta) {
    for (auto circ : circs) {
        circ.increaseR(delta);
    }
}

/**
 * Questa funzione prende in ingresso una coda di pacchetti con lo stesso FCS(hash) e provenienti da schede differenti.
 * L'obiettivo è restituire la posizione stimata. Restituisce PositionData(-1,-1) se dato non va bene.
 * @param deque
 * @return
 */
PositionData MonitoringServer::fromRssiToXY(const std::deque<Packet> &deque) {
    std::deque<Circle> circles{};
    // Deque che ci serve per media pesata
    std::deque<std::pair<PositionData, double>> pointW;
    int retry = 0;
    bool error = true;
    int delta = 0;
    auto backup_i = circles.begin();
    auto backup_j = circles.begin() + 1;

    // Se i cerchi non si intersecano si va ad aumentare il modulo dell'RSSI per poter raggiungere una migliore stima
    // della posizione fino ad un massimo di 1000 volte
    while (retry < 1000 && error) {
        // Pulizia deque cerchi
        circles.clear();
        // Da pacchetti a Cerchi di centro schedina e raggio RSSI -> metri meno il numero di retry finora
        for (auto &packet: deque) {
            auto b = boards.find(packet.getIdSchedina());
            if (b == boards.end()) return PositionData::positionDataNull();
            double dist = calculateDistance(packet.getRssi() + delta, b->second.getA());
            Circle res{dist, b->second.getCoord().x(), b->second.getCoord().y()};
            circles.push_back(res);
        }
        // Ipotesi no errori
        error = false;
        // Combinazioni  e controllo, se anche una non è soddisfatta allora ingrandisco tutte le circonferenze dello spazio che manca.
        for (auto circle_i = circles.begin(); circle_i != circles.end() - 1; circle_i++) {
            for (auto circle_j = circle_i + 1; circle_j != circles.end(); circle_j++) {
                // Punti di intersezione
                Point2d intPoint1, intPoint2;
                // Calcolare intersezione
                int i_points = circle_i->intersect(*circle_j, intPoint1, intPoint2);

                if (i_points == -1) {
                    // Cerchi coincidenti (-1) scarta tutto, qualcosa non quadra

                    return PositionData::positionDataNull();
                }
                if (i_points == -2) {
                    //  Cerchi non coincidenti e contenuti uno nell'altro (-2) riduco il raggio aumentando RSSI
                    // Caso limite collasso circonferenza
                    if (circle_i->getR() < 0.5) {
                        intPoint1 = circle_i->getC();
                        continue;
                    }
                    if (circle_j->getR() < 0.5) {
                        intPoint1 = circle_j->getC();
                        continue;
                    }
                    error = true;
                    delta += 1;
                    break;
                }
                if (i_points == 0) {
                    // Cerchi non si toccano (0) aumento il raggio diminuendo RSSI
                    error = true;
                    delta += -1;
                    break;
                }
            }
            // Se trovo un errore interrompo il for
            if (error) break;
        }
        // Rieseguo il while aumentando il numero di retry.
        if (error) retry++;
    }
    // Se dopotutto l'errore persiste restituisco PositionData nullo.
    if (error) return PositionData::positionDataNull();

    // Doppio ciclo per combinazioni e calcolo
    for (auto circle_i = circles.begin(); circle_i != circles.end() - 1; circle_i++) {
        for (auto circle_j = circle_i + 1; circle_j != circles.end(); circle_j++) {
            // Punti di intersezione
            Point2d intPoint1, intPoint2;
            // Calcolare intersezione
            size_t i_points = circle_i->intersect(*circle_j, intPoint1, intPoint2);
            if (circle_i->getR() < 0.5 && i_points == -2) {
                intPoint1 = circle_i->getC();
            } else if (circle_j->getR() < 0.5 && i_points == -2) {
                intPoint1 = circle_j->getC();
            } else if (i_points < 1) return PositionData::positionDataNull();
            // Controllo se questi punti sono contenuti nelle altre circonferenze.
            // Ipotesi verificata a meno che non si trovi una condizione falsa.
            bool ok1 = true;
            bool ok2 = i_points > 1;
            for (auto circle_k = circles.begin(); circle_k != circles.end(); circle_k++) {
                // Non controllare su circonferenze oggetto di intersezione
                if (circle_k == circle_i || circle_k == circle_j) continue;

                if (!circle_k->containsPoint(intPoint1)) {
                    ok1 = false;
                }
                if (i_points > 1) {
                    if (!circle_k->containsPoint(intPoint2)) {
                        ok2 = false;
                    }
                }
            }

            if (ok1) {
                // CALCOLO CON MEDIA PESATA
                PositionData mean_res{};
                mean_res.addPacket(intPoint1.x(), intPoint1.y());
                pointW.emplace_back(mean_res, circle_i->getR() + circle_j->getR());
            }
            if (ok2) {
                // CALCOLO CON MEDIA PESATA
                PositionData mean_res{};
                mean_res.addPacket(intPoint2.x(), intPoint2.y());
                pointW.emplace_back(mean_res, circle_i->getR() + circle_j->getR());
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

    return result;
}

/**
 * Questa funzione prende in ingresso una coda di pacchetti con lo stesso FCS(hash) e provenienti da schede differenti.
 * L'obiettivo è restituire la posizione stimata. Restituisce PositionData(-100,-100) se dato non va bene.
 * @param deque
 * @return
 */
PositionData MonitoringServer::trilateration(const std::deque<Packet> &deque) {
    // Deque che ci serve per media pesata
    std::deque<std::pair<PositionData, double>> pointW;
    int retry = 0;
    int delta = 0;
    // Combinazioni di pacchetti
    for (auto p_i = deque.begin(); p_i != deque.end() - 1; p_i++) {
        for (auto p_j = p_i + 1; p_j != deque.end(); p_j++) {
            // RSSI da aumentare o diminuire rispetto al valore originale
            delta = 0;
            // Tentativi limite
            retry = 0;

            // Controllo se appartengono a schedine configurate
            auto b_i = boards.find(p_i->getIdSchedina());
            auto b_j = boards.find(p_j->getIdSchedina());
            if (b_i == boards.end() || b_j == boards.end()) return PositionData::positionDataNull();

            double radius_i = calculateDistance(p_i->getRssi(), b_i->second.getA());
            double radius_j = calculateDistance(p_j->getRssi(), b_j->second.getA());
            // Da pacchetti a Cerchi di centro schedina e raggio RSSI -> metri meno il numero di retry finora
            double dist_i = calculateDistance(p_i->getRssi() + delta, b_i->second.getA());
            double dist_j = calculateDistance(p_j->getRssi() + delta, b_j->second.getA());
            Circle c1{dist_i, b_i->second.getCoord().x(), b_i->second.getCoord().y()};
            Circle c2{dist_j, b_j->second.getCoord().x(), b_j->second.getCoord().y()};

            // Punti di intersezione
            Point2d intPoint1, intPoint2;
            // Calcolare intersezione
            int i_points;
            while ((i_points = c1.intersect(c2, intPoint1, intPoint2)) <= 0 && retry < 1000) {
                // Caso limite collasso circonferenza
                if (c1.getR() < 0.2 && i_points == -2) {
                    intPoint1 = c1.getC();
                    i_points = 1;
                    break;
                }
                if (c2.getR() < 0.2 && i_points == -2) {
                    intPoint1 = c2.getC();
                    i_points = 1;
                    break;
                }

                if (i_points == 0) {
                    // Increase radius -> Decrease RSSI
                    delta += -1;
                    dist_i = calculateDistance(p_i->getRssi() + delta, b_i->second.getA());
                    dist_j = calculateDistance(p_j->getRssi() + delta, b_j->second.getA());
                    Circle c1_new{dist_i, b_i->second.getCoord().x(), b_i->second.getCoord().y()};
                    Circle c2_new{dist_j, b_j->second.getCoord().x(), b_j->second.getCoord().y()};
                    c1 = c1_new;
                    c2 = c2_new;
                } else if (i_points == -2) {
                    // Decrease radius -> Increase RSSI
                    delta += +1;
                    dist_i = calculateDistance(p_i->getRssi() + delta, b_i->second.getA());
                    dist_j = calculateDistance(p_j->getRssi() + delta, b_j->second.getA());
                    Circle c1_new{dist_i, b_i->second.getCoord().x(), b_i->second.getCoord().y()};
                    Circle c2_new{dist_j, b_j->second.getCoord().x(), b_j->second.getCoord().y()};
                    c1 = c1_new;
                    c2 = c2_new;
                }
                retry++;

            }
            if (retry >= 1000) {
                return PositionData::positionDataNull();
            }
            if (i_points == 2) {
                // Con due punti di intersezione prendiamo quello più vicino ad entrambi i punti rimanenti
                for (auto p_k = deque.begin(); p_k != deque.end(); p_k++) {
                    if (p_k == p_i || p_k == p_j) continue;
                    auto b_k = boards.find(p_k->getIdSchedina());
                    if (b_k == boards.end()) return PositionData::positionDataNull();
                    Point2d coo = b_k->second.getCoord();
                    double d_1 = coo.distance(intPoint1);
                    double d_2 = coo.distance(intPoint2);
                    if (d_1 < d_2) {
                        PositionData p{intPoint1.x(), intPoint1.y()};
                        std::cout << p;
                        pointW.emplace_back(p, c1.getR() + c2.getR());
                    } else {
                        PositionData p{intPoint2.x(), intPoint2.y()};
                        pointW.emplace_back(p, c1.getR() + c2.getR());
                    }
                }
//                PositionData p{(intPoint1.x() + intPoint2.x()) / 2, (intPoint1.y() + intPoint2.y()) / 2};
//                pointW.emplace_back(p, c1.getR() + c2.getR());
            } else if (i_points == 1) {
                PositionData p{intPoint1.x(), intPoint1.y()};
                pointW.emplace_back(p, radius_i + radius_j);
            }
        }
    }
    std::cout << std::endl;
    // CALCOLO FINALE MEDIA PESATA
    double num_x = 0;
    double num_y = 0;
    double den = 0;
    std::for_each(pointW.begin(), pointW.end(),
                  [&](std::pair<PositionData, double> pair) {
                        std::cout << pair.first.getStringPosition() << std::endl;
                      num_x += pair.first.getX() / pair.second;
                      num_y += pair.first.getY() / pair.second;
                      den += 1 / pair.second;
                  });

    PositionData result{};
    result.addPacket(num_x / den, num_y / den);

    return result;
}


/**
 * Questa funzione prende in ingresso una coda di pacchetti con lo stesso FCS(hash) e provenienti da schede differenti.
 * L'obiettivo è restituire la posizione stimata. Restituisce PositionData(-100,-100) se dato non va bene.
 * @param deque
 * @return
 */
PositionData MonitoringServer::near(const std::deque<Packet> &deque) {
    // Deque che ci serve per media pesata
    std::deque<std::pair<PositionData, double>> pointW;
    int retry = 0;
    int delta = 0;
    // Combinazioni di pacchetti
    for (auto p_i = deque.begin(); p_i != deque.end() - 1; p_i++) {
        for (auto p_j = p_i + 1; p_j != deque.end(); p_j++) {
            // RSSI da aumentare o diminuire rispetto al valore originale
            delta = 0;
            // Tentativi limite
            retry = 0;

            // Controllo se appartengono a schedine configurate
            auto b_i = boards.find(p_i->getIdSchedina());
            auto b_j = boards.find(p_j->getIdSchedina());
            if (b_i == boards.end() || b_j == boards.end()) return PositionData::positionDataNull();

            double radius_i = calculateDistance(p_i->getRssi(), b_i->second.getA());
            double radius_j = calculateDistance(p_j->getRssi(), b_j->second.getA());
            // Da pacchetti a Cerchi di centro schedina e raggio RSSI -> metri meno il numero di retry finora
            double dist_i = calculateDistance(p_i->getRssi() + delta, b_i->second.getA());
            double dist_j = calculateDistance(p_j->getRssi() + delta, b_j->second.getA());
            Circle c1{dist_i, b_i->second.getCoord().x(), b_i->second.getCoord().y()};
            Circle c2{dist_j, b_j->second.getCoord().x(), b_j->second.getCoord().y()};

            // Punti di intersezione
            Point2d intPoint1, intPoint2;
            // Calcolare intersezione
            int i_points;
            while ((i_points = c1.intersect(c2, intPoint1, intPoint2)) <= 0 && retry < 1000) {
                // Caso limite collasso circonferenza
                if (c1.getR() < 0.2 && i_points == -2) {
                    intPoint1 = c1.getC();
                    i_points = 1;
                    break;
                }
                if (c2.getR() < 0.2 && i_points == -2) {
                    intPoint1 = c2.getC();
                    i_points = 1;
                    break;
                }

                if (i_points == 0) {
                    // Increase radius -> Decrease RSSI
                    delta += -1;
                    dist_i = calculateDistance(p_i->getRssi() + delta, b_i->second.getA());
                    dist_j = calculateDistance(p_j->getRssi() + delta, b_j->second.getA());
                    Circle c1_new{dist_i, b_i->second.getCoord().x(), b_i->second.getCoord().y()};
                    Circle c2_new{dist_j, b_j->second.getCoord().x(), b_j->second.getCoord().y()};
                    c1 = c1_new;
                    c2 = c2_new;
                } else if (i_points == -2) {
                    // Decrease radius -> Increase RSSI
                    delta += +1;
                    dist_i = calculateDistance(p_i->getRssi() + delta, b_i->second.getA());
                    dist_j = calculateDistance(p_j->getRssi() + delta, b_j->second.getA());
                    Circle c1_new{dist_i, b_i->second.getCoord().x(), b_i->second.getCoord().y()};
                    Circle c2_new{dist_j, b_j->second.getCoord().x(), b_j->second.getCoord().y()};
                    c1 = c1_new;
                    c2 = c2_new;
                }
                retry++;

            }
            if (retry >= 1000) {
                return PositionData::positionDataNull();
            }
            if (i_points == 2) {
                // Con due punti di intersezione prendiamo quello più vicino ad entrambi i punti rimanenti
                double d_1 = 0;
                double d_2 = 0;
                for (auto p_k = deque.begin(); p_k != deque.end(); p_k++) {
                    if (p_k == p_i || p_k == p_j) continue;
                    auto b_k = boards.find(p_k->getIdSchedina());
                    if (b_k == boards.end()) return PositionData::positionDataNull();
                    Point2d coo = b_k->second.getCoord();

                    d_1 += coo.distance(intPoint1) - calculateDistance(p_k->getRssi() + delta, b_k->second.getA());
                    d_2 += coo.distance(intPoint2) - calculateDistance(p_k->getRssi() + delta, b_k->second.getA());
                }
                if (d_1 < d_2) {
                    PositionData p{intPoint1.x(), intPoint1.y()};
                    pointW.emplace_back(p, c1.getR() + c2.getR());
                } else {
                    PositionData p{intPoint2.x(), intPoint2.y()};
                    pointW.emplace_back(p, c1.getR() + c2.getR());
                }
//                PositionData p{(intPoint1.x() + intPoint2.x()) / 2, (intPoint1.y() + intPoint2.y()) / 2};
//                pointW.emplace_back(p, c1.getR() + c2.getR());
            } else if (i_points == 1) {
                PositionData p{intPoint1.x(), intPoint1.y()};
                pointW.emplace_back(p, radius_i + radius_j);
            }
        }
    }
    // CALCOLO FINALE MEDIA PESATA
    double num_x = 0;
    double num_y = 0;
    double den = 0;
    std::for_each(pointW.begin(), pointW.end(),
                  [&](std::pair<PositionData, double> pair) {
                      std::cout << pair.first.getStringPosition() << std::endl;
                      num_x += pair.first.getX() / pair.second;
                      num_y += pair.first.getY() / pair.second;
                      den += 1 / pair.second;
                  });

    PositionData result{};
    result.addPacket(num_x / den, num_y / den);

    return result;
}


/**
 * Questa funzione prende in ingresso una coda di pacchetti con lo stesso FCS(hash) e provenienti da schede differenti.
 * L'obiettivo è restituire la posizione stimata. Restituisce PositionData(-100,-100) se dato non va bene.
 * @param deque
 * @return
 */
PositionData MonitoringServer::trilaterationAverage(const std::deque<Packet> &deque) {
    // Deque che ci serve per media pesata
    PositionData res;
    std::deque<std::pair<PositionData, double>> pointW;
    int retry = 0;
    int delta = 0;
    // Combinazioni di pacchetti
    for (auto p_i = deque.begin(); p_i != deque.end() - 1; p_i++) {
        for (auto p_j = p_i + 1; p_j != deque.end(); p_j++) {
            // RSSI da aumentare o diminuire rispetto al valore originale
            delta = 0;
            // Tentativi limite
            retry = 0;

            // Controllo se appartengono a schedine configurate
            auto b_i = boards.find(p_i->getIdSchedina());
            auto b_j = boards.find(p_j->getIdSchedina());
            if (b_i == boards.end() || b_j == boards.end()) return PositionData::positionDataNull();

            // Da pacchetti a Cerchi di centro schedina e raggio RSSI -> metri meno il numero di retry finora
            double dist_i = calculateDistance(p_i->getRssi() + delta, b_i->second.getA());
            double dist_j = calculateDistance(p_j->getRssi() + delta, b_j->second.getA());
            Circle c1{dist_i, b_i->second.getCoord().x(), b_i->second.getCoord().y()};
            Circle c2{dist_j, b_j->second.getCoord().x(), b_j->second.getCoord().y()};

            // Punti di intersezione
            Point2d intPoint1, intPoint2;
            // Calcolare intersezione
            int i_points;
            while ((i_points = c1.intersect(c2, intPoint1, intPoint2)) <= 0 && retry < 1000) {
                // Caso limite collasso circonferenza
                if (c1.getR() < 0.2 && i_points == -2) {
                    intPoint1 = c1.getC();
                    i_points = 1;
                    break;
                }
                if (c2.getR() < 0.2 && i_points == -2) {
                    intPoint1 = c2.getC();
                    i_points = 1;
                    break;
                }

                if (i_points == 0) {
                    // Increase radius -> Decrease RSSI
                    delta += -1;
                    dist_i = calculateDistance(p_i->getRssi() + delta, b_i->second.getA());
                    dist_j = calculateDistance(p_j->getRssi() + delta, b_j->second.getA());
                    Circle c1_new{dist_i, b_i->second.getCoord().x(), b_i->second.getCoord().y()};
                    Circle c2_new{dist_j, b_j->second.getCoord().x(), b_j->second.getCoord().y()};
                    c1 = c1_new;
                    c2 = c2_new;
                } else if (i_points == -2) {
                    // Decrease radius -> Increase RSSI
                    delta += +1;
                    dist_i = calculateDistance(p_i->getRssi() + delta, b_i->second.getA());
                    dist_j = calculateDistance(p_j->getRssi() + delta, b_j->second.getA());
                    Circle c1_new{dist_i, b_i->second.getCoord().x(), b_i->second.getCoord().y()};
                    Circle c2_new{dist_j, b_j->second.getCoord().x(), b_j->second.getCoord().y()};
                    c1 = c1_new;
                    c2 = c2_new;
                }
                retry++;

            }
            if (retry >= 1000) {
                return PositionData::positionDataNull();
            }
            if (i_points == 2) {
                // Con due punti di intersezione prendiamo quello più vicino ad entrambi i punti rimanenti
                for (auto p_k = deque.begin(); p_k != deque.end(); p_k++) {
                    if (p_k == p_i || p_k == p_j) continue;
                    auto b_k = boards.find(p_k->getIdSchedina());
                    if (b_k == boards.end()) return PositionData::positionDataNull();
                    Point2d coo = b_k->second.getCoord();
                    double d_1 = coo.distance(intPoint1);
                    double d_2 = coo.distance(intPoint2);
                    if (d_1 < d_2) {
                        res.addPacket(intPoint1.x(), intPoint1.y());
                    } else {
                        res.addPacket(intPoint2.x(), intPoint2.y());
                    }
                }

            } else if (i_points == 1) {
                res.addPacket(intPoint1.x(), intPoint1.y());
            }
        }
    }
    return res;
}


/**
 * @brief      Conversione molto grossolana da RSSI a Metri
 *
 * @param[in]  rssi  RSSI
 *
 * @return     float in metri
 */
float MonitoringServer::calculateDistance(signed rssi, int A) {
    // n: Costante di propagazione del segnale. Costante 2 in ambiente aperto.
    // A: potenza del segnale ricevuto in dBm ad un metro
    const float cost = settings.value("monitor/n").toFloat();
    return pow(10, (A - rssi) / (10.0 * cost));

}

bool MonitoringServer::start() {
    if (!server.listen(QHostAddress::Any, settings.value("room/port").toInt())) {
        Utility::warningMessage(Strings::SRV_NOT_STARTED, Strings::SRV_NOT_STARTED, Strings::SRV_NOT_STARTED);
        return false;
    } else {
        std::vector<Board> b;
        b = Utility::getBoards();
        boards.clear();
        check_id.clear();
        board_fail.clear();
        std::for_each(b.begin(), b.end(), [&](Board bo) {
            check_id.push_back(bo.getId());
            boards.insert(std::make_pair(bo.getId(), bo));
            board_fail.insert(std::make_pair(bo.getId(), 0));
        });
        QObject::connect(&server, &QTcpServer::newConnection, this, &MonitoringServer::newConnection);
        qInfo() << Strings::SRV_STARTED << server.serverPort();
        QObject::connect(&timer, &QTimer::timeout, this, &MonitoringServer::aggregate);
        timer.start(60000);
        return true;
    }

}

void MonitoringServer::stop() {
    QObject::disconnect(&server, &QTcpServer::newConnection, this, &MonitoringServer::newConnection);
    QObject::disconnect(&timer, &QTimer::timeout, this, &MonitoringServer::aggregate);
    qInfo() << Strings::SRV_STOPPED;

    server.close();
    timer.stop();
}

bool MonitoringServer::is_inside_room(PositionData data) {
    QSettings su{Utility::ORGANIZATION, Utility::APPLICATION};
    return data.getX() >= 0 && data.getY() >= 0 &&
           data.getX() <= settings.value("room/width").toFloat() &&
           data.getY() <= settings.value("room/height").toFloat();
}

void MonitoringServer::newConnection() {
    qDebug() << "New Connection started";
    QTcpSocket *socket = server.nextPendingConnection();
    std::vector<std::string> pacchetti;
    std::deque<Packet> packetsConn;
    std::string allData{};
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
        Utility::split(allData, pacchetti, ';');
        // Conversione in oggetti Packet
        try {
            packetsConn = Utility::string2packet(pacchetti, check_id);
        } catch (const std::invalid_argument &e) {
            qWarning() << e.what();
            socket->flush();
            socket->close();
            qDebug() << "Connection closed";
            delete socket;
            return;
        }

        for (auto &p: packetsConn) {
            DEBUG(p)
            auto it = packets.find(p.getFcs());
            if (it != packets.end()) {
                it->second.first.push_back(p);
            } else {
                std::deque<Packet> l;
                l.push_back(p);
                packets.insert(std::make_pair(p.getFcs(), std::make_pair(l, 0)));
            }
        }
    }

    socket->flush();
    socket->close();
    qDebug() << "Connection closed";
    delete socket;

}

void MonitoringServer::aggregate() {
    qInfo() << Strings::AGG_STARTED;
    // Apro connessione con il database
    bool error = false;
    QSqlDatabase db = Utility::getDB(error);
    if (error) exit(-1);

    // Estrapola numero schedine da vettore
    int nSchedine = boards.size();
    QSet<int> listOfId;             //usato per segnare quali id schedine sono state acquisite
    std::map<std::string, std::deque<Packet>> aggregate{};
    for (auto &packet : packets) {
        std::deque<Packet> no_dup_queue{};
        int id = packet.second.first.at(0).getIdSchedina();
        /*
         * Conteggio per schedine non funzionanti, vado a vedere solo i pacchetti appena inseriti, in questo modo
         * riesco a rilevare immediatamente schedine non funzionanti
         */
        if (packet.second.second == 0) {
            listOfId.insert(id);
        }
        QSet<int> id_board_packet;
        bool to_be_discarded = false;
        std::for_each(packet.second.first.begin(), packet.second.first.end(), [&](Packet &p) {
            if (!id_board_packet.contains(p.getIdSchedina())) {
                id_board_packet.insert(p.getIdSchedina());
                no_dup_queue.push_back(p);
            }
        });
        if (id_board_packet.size() == nSchedine) {
            aggregate.insert(std::make_pair(packet.first, no_dup_queue));
            packet.second.second = 2;
        } else {
            packet.second.second++;
        }
    }

    // Se una schedina non viene rilevata incremento il contatore
    // Ogni volta che viene rilevata nuovamente il contatore viene resettato
    std::for_each(boards.begin(), boards.end(), [&](std::pair<int, Board> pair) {
        if (!listOfId.contains(pair.first)) {
            board_fail.find(pair.first)->second++;
        } else {
            board_fail.find(pair.first)->second = 0;
        }
    });
    // Se una delle schedine non viene rilevata per una quantita eccessiva
    QSet<QString> listBoardFailing{};
    std::for_each(board_fail.begin(), board_fail.end(), [&](std::pair<int, int> pair) {
        DEBUG("Board " + std::to_string(pair.first) + " fail counter: " + std::to_string(pair.second));
        if (pair.second >= Utility::RETRY_STEP_BOARD) {
            listBoardFailing.insert(QString::number(pair.first));
        }
    });
    if (!listBoardFailing.empty()) {
        QString stringOut{"Elenco ID schede offline: [ "};
        int count = 0;
        for (auto &a : listBoardFailing) {
            count++;
            stringOut += "\"" + a + "\"";
            if (count != listBoardFailing.size()) {
                stringOut += ", ";
            }
        }
        stringOut += " ]";

        emit stopped();
        Utility::warningMessage("Schedine non funzionanti",
                                "Ti invitiamo a verificare il funzionamento delle schedine indicate nei dettagli e riavviare il monitoraggio",
                                stringOut);
        qInfo() << Strings::AGG_STOPPED;
    }

    // Stampa id pacchetti aggregati rilevati.
    for (auto fil : aggregate) {
        QSqlQuery query{db};
        query.prepare(
                "INSERT INTO " + settings.value("database/table").toString() +
                " (hash_fcs, mac_addr, pos_x, pos_y, timestamp, ssid, hidden) VALUES (:hash, :mac, :posx, :posy, :timestamp, :ssid, :hidden);");
//            query.bindValue(":id", 0);
        for (auto &p: fil.second) {
            auto b = boards.find(p.getIdSchedina());
            std::cout << b->second.getId() << ": " << p.getRssi() << ", "
                      << this->calculateDistance(p.getRssi(), b->second.getA()) << "; ";
        }
        std::cout << std::endl;
        PositionData positionDataAv = trilateration(fil.second);
        PositionData positionData = near(fil.second);

        std::string packet = "ID packet:" + fil.first + " " + fil.second.begin()->getMacPeer() + " " +
                             positionData.getStringPosition() + " | " + positionDataAv.getStringPosition();
        qDebug() << QString::fromStdString(packet);
        if (positionData == PositionData::positionDataNull()) {
            qDebug() << "Errore Triangolazione";
            continue;
        }
//        if (!is_inside_room(positionData)) {
//            qDebug() << "Fuori dalla stanza";
//            continue;
//        }
        query.bindValue(":hash", QString::fromStdString(fil.second.begin()->getFcs()));
        query.bindValue(":mac", QString::fromStdString(fil.second.begin()->getMacPeer()));
        query.bindValue(":posx", positionData.getX());
        query.bindValue(":posy", positionData.getY());
        query.bindValue(":timestamp", QDateTime::fromSecsSinceEpoch(fil.second.begin()->getTimestamp()));
        query.bindValue(":ssid", QString::fromStdString(fil.second.begin()->getSsid()));
        // Controllo se pacchetto con mac hidden
        if (isRandomMac(fil.second.begin()->getMacPeer())) {
            //mac hidden
            query.bindValue(":hidden", 1);
        } else {
            query.bindValue(":hidden", 0);
        }
        if (!query.exec()) {
            //qDebug() << query.lastError();
            Utility::warningMessage(Strings::ERR_DB,
                                    Strings::ERR_DB_MSG,
                                    query.lastError().text());
            return;
        }
    }
    qInfo() << Strings::AGG_STOPPED;

    // Pulizia deque pacchetti attraverso meccanismo di second chance
    /* Un pacchetto viene eliminato dalla deque solo ed esclusivamente dopo due aggregate. In questo maniera
     * si dovrebbe evitare la possibilità che vengano analizzate ricezioni di pacchetti parziali */
    for (auto it = packets.begin(); it != packets.end();) {
        if (it->second.second >= 2) {
            it = packets.erase(it);
        } else {
            ++it;
        }
    }
    db.close();
    this->aggregated();
}

bool MonitoringServer::isRunning() {
    return server.isListening();
}

bool MonitoringServer::isRandomMac(const std::string &basicString) {
    QString s = QString("0x%1").arg(basicString.at(1));
    std::stringstream ss;
    ss << std::hex << s.toStdString();
    unsigned n;
    ss >> n;
    std::bitset<4> b(n);
//    std::cout << b.to_string() << std::endl;
//    std::cout << b.to_string()[2] << std::endl;
    return b.to_string()[2] == '1';
}