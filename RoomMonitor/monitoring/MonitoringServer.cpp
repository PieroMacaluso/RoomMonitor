//
// Created by pieromack on 26/07/19.
//

#include "MonitoringServer.h"

MonitoringServer::MonitoringServer() {
}

MonitoringServer::~MonitoringServer() {
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

    // Se i cerchi non si intersecano si va ad aumentare il modulo dell'RSSI per poter raggiungere una migliore stima
    // della posizione fino ad un massimo di 1000 volte
    while (retry < 1000 && error) {
        // Pulizia deque cerchi
        circles.clear();
        // Da pacchetti a Cerchi di centro schedina e raggio RSSI -> metri meno il numero di retry finora
        for (auto &packet: deque) {
            auto b = boards.find(packet.getIdSchedina());
            if (b == boards.end()) return PositionData(-1, -1);
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
            if (i_points < 1) return PositionData::positionDataNull();
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

    if (!is_inside_room(result)) return PositionData::positionDataNull();
    return result;
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
    // TODO: vedere se applicabile a stanza
    // A: potenza del segnale ricevuto in dBm ad un metro
    // TODO: da ricercare sperimentalmente
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
        std::for_each(b.begin(), b.end(), [&](Board bo) {
            boards.insert(std::make_pair(bo.getId(), bo));
            board_fail.insert(std::make_pair(bo.getId(), 0));
        });
        QObject::connect(&server, &QTcpServer::newConnection, this, &MonitoringServer::newConnection);
        qInfo() << Strings::SRV_STARTED << server.serverPort();
        QObject::connect(&timer, &QTimer::timeout, this, &MonitoringServer::aggregate);
        timer.start(10000);
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
    QSettings su{Utility::ORGANIZATION, Utility::APPLICATION};
    int margin = su.value("room/margin").toInt();
    return data.getX() >= 0 - margin && data.getY() >= 0 - margin &&
           data.getX() <= settings.value("room/width").toFloat() + margin &&
           data.getY() <= settings.value("room/height").toFloat() + margin;
}

void MonitoringServer::newConnection() {
    qDebug() << "New Connection started";
    std::string startDelim("init");
    std::string stopDelim("end");
    QTcpSocket *socket = server.nextPendingConnection();
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
        Utility::split(allData, pacchetti, ';');
        // Conversione in oggetti Packet
        packetsConn = Utility::string2packet(pacchetti);

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

    //todo controllare se va bene un conteiner pacchetti per ogni newConnection (stesso anche in caso la schedina usi più pacchetti tcp per inviare l'intero elenco)
    socket->flush();
    socket->close();
    qDebug() << "Connection closed";
    delete socket;

}

void MonitoringServer::aggregate() {
    qInfo() << Strings::AGG_STARTED;
    bool error = false;
    QSqlDatabase db = Utility::getDB(error);
    if (error) exit(-1);


    // Estrapola numero schedine da vettore
    int nSchedine = boards.size();
    QSet<QString> listOfId;             //usato per segnare quali id schedine sono state acquisite
    std::map<std::string, std::deque<Packet>> aggregate{};
    for (auto it = packets.begin(); it != packets.end(); it++) {
        std::string id = it->first;
        listOfId.insert(QString::fromStdString(id));
        if (it->second.first.size() == nSchedine) {
            aggregate.insert(std::make_pair(id, it->second.first));
            it->second.second = 2;
        } else {
            it->second.second++;
        }
    }
    if (listOfId.size() < nSchedine) {
        // Se una schedina non viene rilevata incremento il contatore
        // Ogni volta che viene rilevata nuovamente il contatore viene resettato
        std::for_each(boards.begin(), boards.end(), [&](std::pair<int, Board> pair) {
            if (!listOfId.contains(QString::number(pair.first))) {
                board_fail.find(pair.first)->second++;
            } else {
                board_fail.find(pair.first)->second = 0;
            }
        });
        // Se una delle schedine non viene rilevata per una quantita eccessiva
        QSet<QString> listBoardFailing{};
        std::for_each(board_fail.begin(), board_fail.end(), [&](std::pair<int, int> pair) {
            if (pair.second >= Utility::RETRY_STEP_BOARD) {
                listBoardFailing.insert(QString::number(pair.first));
            }
        });
        if (!listBoardFailing.empty()) {
            //qCritical() << "ID schedine mancanti" << listBoardFailing.toList();
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

            Utility::warningMessage("Schedine non funzionanti",
                                    "Ti invitiamo a verificare il funzionamento delle schedine indicate nei dettagli e riavviare il monitoraggio",
                                    stringOut);
            qInfo() << Strings::AGG_STOPPED;
            emit stopped();
        }
        db.close();
        this->aggregated();
        return;
    }

    // Stampa id pacchetti aggregati rilevati.
    for (auto fil : aggregate) {
        /*
         * TODO: Verificare query al database, capire cosa e quanto salvare
         */
        QSqlQuery query{db};
        query.prepare(
                "INSERT INTO " + settings.value("database/table").toString() +
                " (hash_fcs, mac_addr, pos_x, pos_y, timestamp, ssid, hidden) VALUES (:hash, :mac, :posx, :posy, :timestamp, :ssid, :hidden);");
//            query.bindValue(":id", 0);
        PositionData positionData = fromRssiToXY(fil.second);
        std::string packet = "ID packet:" + fil.first + " " + fil.second.begin()->getMacPeer() + " " +
                             positionData.getStringPosition();
        qDebug() << QString::fromStdString(packet);
        if (positionData == PositionData::positionDataNull()) continue;
        query.bindValue(":hash", QString::fromStdString(fil.second.begin()->getFcs()));
        query.bindValue(":mac", QString::fromStdString(fil.second.begin()->getMacPeer()));
        query.bindValue(":posx", positionData.getX());
        query.bindValue(":posy", positionData.getY());
        query.bindValue(":timestamp", QDateTime::fromSecsSinceEpoch(fil.second.begin()->getTimestamp()));
        query.bindValue(":ssid", QString::fromStdString(fil.second.begin()->getSsid()));
        // TODO: controllare bene
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