//
// Created by pieromack on 26/07/19.
//

#include <QtCore/QSettings>
#include <QtSql/QSqlRecord>
#include <bitset>
#include "MonitoringServer.h"
#include "Circle.h"
#include "../windows/SettingDialog.h"

MonitoringServer::MonitoringServer() {
}

MonitoringServer::~MonitoringServer() {
}


std::deque<Packet> MonitoringServer::string2packet(const std::vector<std::string> &p) {
    std::deque<Packet> deque;
    std::string ssid;

    for (const std::string &s:p) {
        std::vector<std::string> values;
        split(s, values, ',');

        if (values[5] == "~")        //alcuni pacchetti contengono l'ssid e altri no
            //no ssid
            ssid = "Nan";
        else
            ssid = values[5];

        Packet packet(std::stoi(values[0]), values[1], std::stoi(values[2]), values[3], std::stoi(values[4]), ssid);
        //std::cout<<"pacchetto "<<stoi(values[0]) <<" "<<values[3]<<" "<<std::stoi(values[4])<<" "<<values[5]<<" "<<std::stoi(values[6])<<" "<< ssid;
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
PositionData MonitoringServer::fromRssiToXY(const std::deque<Packet>& deque) {
    // Deque di cerchi
    std::deque<Circle> circles{};
    // Deque che ci serve per media pesata
    std::deque<std::pair<PositionData, double>> pointW;
    int retry = 0;
    bool error = true;
    int delta = 0;

    /**
     * Se i cerchi non si intersecano si va ad aumentare il modulo dell'RSSI per poter raggiungere una migliore stima
     * della posizione fino ad un massimo di 1000 volte
     */
    while (retry < 1000 && error) {
        // Pulizia deque cerchi
        circles.clear();
        // Da pacchetti a Cerchi di centro schedina e raggio RSSI->metri meno il numero di retry finora
        for (auto &packet: deque) {
            auto b = boards.find(packet.getIdSchedina());
            if (b == boards.end()) return PositionData(-1, -1);
            double dist = calculateDistance(packet.getRssi() + delta);
            Circle res{dist, b->second.getCoord().x(), b->second.getCoord().y()};
            circles.push_back(res);
        }
        // Ipotesi no errori
        error = false;
        // Combinazioni  e controllo, se anche una non è soddisfatta allora ingrandisco tutte le circonferenze dello spazio che manca.
        for (int i = 0; i < circles.size() - 1; i++) {
            for (int j = i + 1; j < circles.size(); j++) {
                // Punti di intersezione
                Point2d intPoint1, intPoint2;

                // Calcolare intersezione
                int i_points = circles[i].intersect(circles[j], intPoint1, intPoint2);

                // TODO: Controllare funzionalità
                // Se cerchi coincidenti(-1) scarta tutto, qualcosa non quadra
                if (i_points == -1) return PositionData{-1, -1};
                //  Se cerchi non coincidenti econtenuti uno nell'altro (-2) riduco il raggio modificando RSSI
                if (i_points == -2) {
                    error = true;
                    delta += 1;
                    break;
                }
                // Se cerchi non si toccano aumento il raggio modificando RSSI
                if (i_points == 0) {
                    error = true;
                    delta += -1;
                    break;
                }
            }
            if (error) break;
        }
        if (error) retry++;
    }
    if (error) return PositionData{-1, -1};

    // Doppio ciclo per combinazioni e calcolo
    for (int i = 0; i < circles.size() - 1; i++) {
        for (int j = i + 1; j < circles.size(); j++) {
            // Punti di intersezione
            Point2d intPoint1, intPoint2;
            // Calcolare intersezione
            // TODO: rivedere bene funzione intersect
            size_t i_points = circles[i].intersect(circles[j], intPoint1, intPoint2);
            if (i_points < 1) return PositionData{-1, -1};
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
    const float A = settings.value("monitor/A").toFloat();
    // A: potenza del segnale ricevuto in dBm ad un metro
    // TODO: da ricercare sperimentalmente
    const float cost = settings.value("monitor/n").toFloat();

    return pow(10, (A - rssi) / (10.0 * cost));

}

void MonitoringServer::start() {

    if (!server.listen(QHostAddress::Any, settings.value("room/port").toInt())) {
        qDebug() << "Server Did not start";
    } else {
        boards.clear();
        boards = Board::extract_from_setting();
        QObject::connect(&server, &QTcpServer::newConnection, this, &MonitoringServer::newConnection);
        qDebug() << "Server Started on port:" << server.serverPort();
        QObject::connect(&timer, &QTimer::timeout, this, &MonitoringServer::aggregate);
        timer.start(30000);
    }

}

void MonitoringServer::stop() {
    QObject::disconnect(&server, &QTcpServer::newConnection, this, &MonitoringServer::newConnection);
    QObject::disconnect(&timer, &QTimer::timeout, this, &MonitoringServer::aggregate);
    qDebug() << "Server Disconnected:" << server.serverPort();

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
    // TODO: margine?
    return data.getX() >= 0 && data.getY() >= 0 && data.getX() <= settings.value("room/width").toFloat() &&
           data.getY() <= settings.value("room/height").toFloat();
}

void MonitoringServer::newConnection() {
    DEBUG("New Connection started");
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
        MonitoringServer::split(allData, pacchetti, ';');
        // Conversione in oggetti Packet
        packetsConn = string2packet(pacchetti);

        // TODO: Verificare reale necessità di thread-safeness, altrimenti liberare tutto
        /** INIZIO ESECUZIONE THREAD-SAFE */

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
    DEBUG("Connection closed")
    delete socket;

}

void MonitoringServer::aggregate() {
    DEBUG("Starting aggregation")
    QSqlDatabase db = QSqlDatabase::database();
    if (db.isOpenError()) {
        qDebug() << db.lastError();
        exit(-1);
    }

    // TODO: Capire come aggregare bene

    // Estrapola numero schedine da vettore
    int nSchedine = boards.size();

    std::map<std::string, std::deque<Packet>> aggregate{};
    for (auto it = packets.begin(); it != packets.end(); it++) {
        std::string id = it->first;
        if (it->second.first.size() == nSchedine) {
            aggregate.insert(std::make_pair(id, it->second.first));
            it->second.second = 2;
        } else {
            it->second.second++;
        }
    }
    // Stampa id pacchetti aggregati rilevati.
    for (auto fil : aggregate) {
        /*
         * TODO: Verificare query al database, capire cosa e quanto salvare
         */
        QSqlQuery query{};
        query.prepare(
                "INSERT INTO " + settings.value("database/table").toString() +
                " (hash_fcs, mac_addr, pos_x, pos_y, timestamp, ssid, hidden) VALUES (:hash, :mac, :posx, :posy, :timestamp, :ssid, :hidden);");
//            query.bindValue(":id", 0);
        PositionData positionData = fromRssiToXY(fil.second);
        DEBUG("ID packet:" << fil.first << " " << fil.second.begin()->getMacPeer()<< " " << positionData.getX() << " " << positionData.getY());
        if (positionData.getX() == -1 || positionData.getY() == -1) continue;
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
            qDebug() << query.lastError();
        }
    }
    DEBUG("Ending aggregation")

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

/**
 * Funzione utilizzata per ottenere il pacchetto con il maggior timestamp di un determinato mac
 * @param mac
 * @param initTime
 * @param endTime
 * @return
 */
Packet MonitoringServer::getLastPacketWithMac(QString mac, uint32_t initTime, uint32_t endTime){
    QSqlDatabase db= QSqlDatabase::database();
    QSqlQuery query{};
    QString table="stanza";         //todo vedere da impostazioni
    QDateTime timeInit;
    QDateTime timeEnd;

    timeInit.setTime_t(initTime);   //todo verificare problema fusi, inserisco il timestamp delle 18.00 ma lo trasforma in 20.00. Potrebbe essere un problema per la query
    timeEnd.setTime_t(endTime);

    query.prepare("SELECT * FROM "+table+" WHERE mac_addr='"+ mac+"' AND timestamp>='" + timeInit.toUTC().toString("yyyy-MM-dd hh:mm:ss") + "' AND timestamp<='" + timeEnd.toUTC().toString("yyyy-MM-dd hh:mm:ss") + "' ORDER BY TIMESTAMP desc;");

    if (!query.exec()) {
        qDebug() << query.lastError();
    }

    if (query.size() == 0){
        Packet p(-1, "null", -1, (std::string &) "null", -1, (std::string &) "null");
        return p;
    }

    while(query.next()){
        std::string fcs = query.value(1).toString().toStdString();
        std::string mac = query.value(2).toString().toStdString();
        uint32_t timestamp = query.value(5).toDateTime().toSecsSinceEpoch();
        std::string ssid = query.value(6).toString().toStdString();

        Packet p(-1, fcs, -1, mac, timestamp, ssid);

        double_t posX = query.value(3).toDouble();
        double_t posY = query.value(4).toDouble();
        PositionData positionData(posX, posY);
        p.setPosition(positionData);

        return p;
    }


}

std::deque<Packet> MonitoringServer::getHiddenPackets(uint32_t initTime, uint32_t endTime) {
    //query per ottenere i pacchetti con mac hidden nel periodo specificato
    QDateTime timeInit;
    QDateTime timeEnd;

    timeInit.setTime_t(initTime);   //todo verificare problema fusi, inserisco il timestamp delle 18.00 ma lo trasforma in 20.00. Potrebbe essere un problema per la query
    timeEnd.setTime_t(endTime);


    std::deque<Packet> hiddenPackets;
    QString table = "stanza";         //todo vedere da impostazioni
    QSqlQuery query{};
    query.prepare("SELECT * FROM " + table + " WHERE hidden='1' AND timestamp>='" +
                  timeInit.toUTC().toString("yyyy-MM-dd hh:mm:ss") + "' AND timestamp<='" +
                  timeEnd.toUTC().toString("yyyy-MM-dd hh:mm:ss") + "';");
    qDebug() << query.executedQuery();
    if (!query.exec()) {
        qDebug() << query.lastError();
    }
    QSqlRecord record = query.record();
    if (query.size() == 0)
        qDebug() << "Nessun risultato";

    while (query.next()) {                                //ciclo su ogni entry selezionata del db
        std::string fcs = query.value(1).toString().toStdString();
        std::string mac = query.value(2).toString().toStdString();
        uint32_t timestamp = query.value(5).toDateTime().toSecsSinceEpoch();
        std::string ssid = query.value(6).toString().toStdString();

        Packet p(-1, fcs, -1, mac, timestamp, ssid);

        double_t posX = query.value(3).toDouble();
        double_t posY = query.value(4).toDouble();
        PositionData positionData(posX, posY);
        p.setPosition(positionData);
        hiddenPackets.push_back(p);

    }
    //todo ordinare per timestamp, utile per getHiddenMacFor
    return hiddenPackets;
}


/**
* veririca se sono presenti mac simili a source oppure no. Mac selezionato da interfaccia. Vengono ottenuti dal db tutti i pacchetti hidden nel lasso di tempo specificato.
* @param source
* @param initTime
* @param endTime
* @return
*/
//todo dovrebbe essere sostituita con getHiddenMacFor
bool MonitoringServer::getHiddenDeviceFor(Packet source, uint32_t initTime, uint32_t endTime,
                                          std::deque<Packet> &hiddenPackets) {
    //entro 5 minuti, stessa posizione +-0.5, altro da vedere
    uint32_t tolleranzaTimestamp = 240;//usata per definire entro quanto la posizione deve essere uguale, 240= 4 minuti
    double tolleranzaX = 0.5;     //todo valutare se ha senso impostare le tolleranze da impostazioni grafiche
    double tolleranzaY = 0.5;
    double perc;
    bool trovato = false;

    /*std::deque<Packet> hiddenPackets=getHiddenPackets(initTime,endTime);
    if(hiddenPackets.size()==0)
        return false;*/

    for (int j = 0; j < hiddenPackets.size(); j++) {
        if (hiddenPackets.at(j).getMacPeer() != source.getMacPeer()) {
            double diff = (source.getTimestamp() < hiddenPackets.at(j).getTimestamp()) ? (
                    hiddenPackets.at(j).getTimestamp() - source.getTimestamp()) : (source.getTimestamp() -
                                                                                   hiddenPackets.at(j).getTimestamp());
            if (diff <= tolleranzaTimestamp) {
                //mac diverso ad intervallo inferiore di 1 minuto
                double diffX = (source.getX() < hiddenPackets.at(j).getX()) ? (hiddenPackets.at(j).getX() -
                                                                               source.getX()) : (source.getX() -
                                                                                                 hiddenPackets.at(
                                                                                                         j).getX());
                double diffY = (source.getY() < hiddenPackets.at(j).getY()) ? (hiddenPackets.at(j).getY() -
                                                                               source.getY()) : (source.getY() -
                                                                                                 hiddenPackets.at(
                                                                                                         j).getY());
                if (diffX <= tolleranzaX && diffY <= tolleranzaY) {
                    //mac diverso con posizione simile in 4 minuto=> possibile dire che sia lo stesso dispositivo
                    perc = (100 - ((diffX * 100 / tolleranzaX) + (diffY * 100 / tolleranzaY) +
                                   (diff * 100 / tolleranzaTimestamp)) * 100 / (300));
                    std::cout << source.getMacPeer() << " simile ad " << hiddenPackets.at(j).getMacPeer()
                              << " con probabilita' del " << perc << "%" << std::endl;
                    //todo decidere cosa fare con tale percentiale
                    trovato = true;
                }
            }

        }
    }

    return trovato;
}

/**
 * Stessa cosa della funzione getHiddenDeviceFor ma questa volta partendo solo dalla stringa del mac e non da un intero pacchetto
 * @param mac
 * @param initTime
 * @param endTime
 * @return
 */
bool MonitoringServer::getHiddenMacFor(QString mac, uint32_t initTime, uint32_t endTime) {
    //entro 5 minuti, stessa posizione +-0.5, altro da vedere
    uint32_t tolleranzaTimestamp = 240;//usata per definire entro quanto la posizione deve essere uguale, 240= 4 minuti
    double tolleranzaX = 0.5;     //todo valutare se ha senso impostare le tolleranze da impostazioni grafiche
    double tolleranzaY = 0.5;

    double perc;
    bool trovato = false;

    std::deque<Packet> hiddenPackets=getHiddenPackets(initTime,endTime);                                //ottiene tutti i pacchetti dello stesso mac in quel determinato intervallo
    if(hiddenPackets.empty())
        return false;                                                                                    //nel lasso di tempo scelto non è stato trovato nessun pacchetto con il mac selezionato. Per effettuare la stima è necessario che ci sia almeno un pacchetto con  il mac scelto

    std::list<Packet> allPacketsOfMac=getAllPacketsOfMac(mac,initTime,endTime);
    if(allPacketsOfMac.empty())
        return false;

    for (int j = 0; j < hiddenPackets.size(); j++) {
        if (hiddenPackets.at(j).getMacPeer() != mac.toStdString()) {

            double_t maxPerc=0;

            for(Packet source:allPacketsOfMac){                                                           //controlla tutti i mac nascosti con tutte le posizioni del mac scelto nell'intervallo selezionato
                double diff = (source.getTimestamp() < hiddenPackets.at(j).getTimestamp()) ? (
                        hiddenPackets.at(j).getTimestamp() - source.getTimestamp()) : (source.getTimestamp() -
                                                                                       hiddenPackets.at(j).getTimestamp());
                if (diff <= tolleranzaTimestamp) {
                    //mac diverso ad intervallo inferiore di 1 minuto
                    double diffX = (source.getX() < hiddenPackets.at(j).getX()) ? (hiddenPackets.at(j).getX() -
                                                                                   source.getX()) : (source.getX() -
                                                                                                     hiddenPackets.at(
                                                                                                             j).getX());
                    double diffY = (source.getY() < hiddenPackets.at(j).getY()) ? (hiddenPackets.at(j).getY() -
                                                                                   source.getY()) : (source.getY() -
                                                                                                     hiddenPackets.at(
                                                                                                             j).getY());
                    if (diffX <= tolleranzaX && diffY <= tolleranzaY) {
                        //mac diverso con posizione simile in 4 minuto=> possibile dire che sia lo stesso dispositivo
                        perc = (100 - ((diffX * 100 / tolleranzaX) + (diffY * 100 / tolleranzaY) +
                                       (diff * 100 / tolleranzaTimestamp)) * 100 / (300));
                        if(perc>maxPerc)
                            maxPerc=perc;
                    }
                }
            }
            std::cout << mac.toStdString() << " simile ad " << hiddenPackets.at(j).getMacPeer()
                      << " con probabilita' del " << maxPerc << "%" << std::endl;

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
//getHiddenDevice(1569088800,1569091920);
int MonitoringServer::getHiddenDevice(uint32_t initTime, uint32_t endTime) {
    bool trovato;
    int numHiddenDevice = 0;


    std::deque<Packet> hiddenPackets = getHiddenPackets(initTime, endTime);
    if (hiddenPackets.empty())
        return 0;

    for (int i = 0; i < hiddenPackets.size(); i++) {
        trovato = getHiddenDeviceFor(hiddenPackets.at(i), initTime, endTime, hiddenPackets);
        if (trovato)
            numHiddenDevice++;
    }

    qDebug() << "Numero di dispositivi differenti con mac nascosto: " << numHiddenDevice;
    return numHiddenDevice;
    //todo decidere cosa fare con tale numero
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

/**
 * Funzione che ottiene tutti i pacchetti di un determinato mac in un determinato intervallo temporale
 * @param mac
 * @param initTime
 * @param endTime
 * @return
 */
std::list<Packet> MonitoringServer::getAllPacketsOfMac(const QString& mac, uint32_t initTime, uint32_t endTime) {
    QSqlDatabase db= QSqlDatabase::database();
    QSqlQuery query{};
    QString table="stanza";         //todo vedere da impostazioni
    QDateTime timeInit;
    QDateTime timeEnd;

    timeInit.setTime_t(initTime);   //todo verificare problema fusi, inserisco il timestamp delle 18.00 ma lo trasforma in 20.00. Potrebbe essere un problema per la query
    timeEnd.setTime_t(endTime);

    query.prepare("SELECT * FROM "+table+" WHERE mac_addr='"+ mac+"' AND timestamp>='" + timeInit.toUTC().toString("yyyy-MM-dd hh:mm:ss") + "' AND timestamp<='" + timeEnd.toUTC().toString("yyyy-MM-dd hh:mm:ss") + "';");

    if (!query.exec()) {
        qDebug() << query.lastError();
    }

    if (query.size() == 0){
       return std::list<Packet>();
    }

    std::list<Packet> allPacketsOfMac;
    while(query.next()){
        std::string fcs = query.value(1).toString().toStdString();
        std::string mac_add = query.value(2).toString().toStdString();
        uint32_t timestamp = query.value(5).toDateTime().toSecsSinceEpoch();
        std::string ssid = query.value(6).toString().toStdString();

        Packet p(-1, fcs, -1, mac_add, timestamp, ssid);

        double_t posX = query.value(3).toDouble();
        double_t posY = query.value(4).toDouble();
        PositionData positionData(posX, posY);
        p.setPosition(positionData);

        allPacketsOfMac.push_back(p);
    }

    return allPacketsOfMac;

}







