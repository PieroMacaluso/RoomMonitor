//
// Created by pieromack on 26/07/19.
//

#include <QtCore/QSettings>
#include <QtSql/QSqlRecord>
#include "MonitoringServer.h"
#include "Circle.h"
#include "../windows/SettingDialog.h"

MonitoringServer::MonitoringServer() {
    nDatabase = QSqlDatabase::addDatabase("QMYSQL", "reception");
}

MonitoringServer::~MonitoringServer() {
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

            // TODO: Trovare soluzione per cerchi coincidenti(-1) o cerchi contenuti uno nell'altro(-2)
            if (i_points < 0) return PositionData{-1, -1};
            // Se non si intersecano TODO: vedere 1 o 0
            if (i_points <= 1) {
                // Calcolo distanza centri diviso due + margine
                double margine = 0.5;
                double delta = intPoint1.distance(intPoint2) / 2 + margine;
                // Aggiungo questo delta a tutti i cerchi
                for (auto mod : circles) mod.increaseR(delta);
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
    const float cost = settings.value("monitor/n").toFloat();
    // A: potenza del segnale ricevuto in dBm ad un metro
    // TODO: da ricercare sperimentalmente
    const float A = settings.value("monitor/A").toFloat();

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

void MonitoringServer::start() {
    nDatabase = QSqlDatabase::addDatabase("QMYSQL");
    nDatabase.setHostName(settings.value("database/host").toString());
    nDatabase.setDatabaseName(settings.value("database/name").toString());
    nDatabase.setPort(settings.value("database/port").toInt());
    nDatabase.setUserName(settings.value("database/user").toString());
    nDatabase.setPassword(settings.value("database/pass").toString());

    getHiddenDevice(1569088800,1569091920);

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
    return data.getX() >= 0 && data.getY() >= 0 && data.getX() <= settings.value("room/height").toFloat() &&
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

        std::unique_lock lk{m};
        for (auto &p: packetsConn) {
            DEBUG(p)
            packets.emplace_back(std::make_pair(p, 0));
        }

        /** FINE ESECUZIONE THREAD-SAFE */
    }

    //todo controllare se va bene un conteiner pacchetti per ogni newConnection (stesso anche in caso la schedina usi più pacchetti tcp per inviare l'intero elenco)
    socket->flush();
    socket->close();
    DEBUG("Connection closed")
    delete socket;

}

void MonitoringServer::aggregate() {

    connectDB();

    // TODO: Capire come aggregare bene

    std::unique_lock lk{m};

    // Estrapola numero schedine da vettore
    int nSchedine = boards.size();

    // Generazione Mappa <ChiavePacchetto, DequePacchetto>

    /* TODO: capire se si può fare direttamente in acquisizione, secondo me si può usare la mappa in maniera thread safe
     * in questo modo si andrebbe a creare l'aggregazione per id pacchetto già in ricezione, limitando lo sforzo
     * computazionale che viene fatto in aggregate*/

    std::map<std::string, std::deque<Packet>> aggregate{};
    std::for_each(packets.begin(), packets.end(), [&](std::pair<Packet, int> el) {
        std::string id = el.first.getFcs();
        el.second++;
        auto it = aggregate.find(id);
        if (it == aggregate.end()) {
            std::deque<Packet> newDeque{};
            newDeque.push_back(el.first);
            aggregate.insert(std::make_pair(id, newDeque));
        } else {
            it->second.push_back(el.first);
        }
    });

    // Eliminazione di tutti i pacchetti che non possiedono un numero di elementi pari al numero di schedine.
    for (auto i = aggregate.begin(), last = aggregate.end(); i != last;) {
        if (nSchedine != i->second.size()) {
            i = aggregate.erase(i);
        } else {

            for (auto it = packets.begin(); it != packets.end();) {
                if (it->first.getFcs() == i->first) {
                    it = packets.erase(it);
                } else {
                    ++it;
                }
            }
            ++i;
        }
    }
    // TODO: verificare inutilità di questa procedura e cancellare
    std::map<std::string, PositionData> map_mac_xy;
    for (auto i = aggregate.begin(), last = aggregate.end(); i != last; i++) {
        std::string mac = i->second.begin()->getMacPeer();
        auto it = map_mac_xy.find(mac);
        if (it == map_mac_xy.end()) {
            // Nuovo MAC
            // Calculate x,y da RSSI
            PositionData positionData = fromRssiToXY(i->second);
            if (positionData.getX() == -1 || positionData.getY() == -1) continue;
            map_mac_xy.insert(std::make_pair(mac, positionData));
            //todo aggiungere posizionData al pacchetto con quel mac prima di salvarlo nel db
        } else {
            // MAC già visto
            // Calculate x,y da RSSI
            PositionData positionData = fromRssiToXY(i->second);
            if (positionData.getX() == -1 || positionData.getY() == -1) continue;
            it->second.addPacket(positionData);
        }
    }


    // Stampa id pacchetti aggregati rilevati.
    DEBUG("Starting aggregation")
    for (auto fil : aggregate) {
        DEBUG("ID packet:" << fil.first << " " << fil.second.begin()->getMacPeer())
        /*
         * TODO: Verificare query al database, capire cosa e quanto salvare
         */
        QSqlQuery query;
        query.prepare(
                "INSERT INTO campi (hash_fcs, mac_addr, pos_x, pos_y, timestamp, ssid, hidden) VALUES (:hash, :mac, :posx, :posy, :timestamp, :ssid, :hidden);");
//            query.bindValue(":id", 0);
        PositionData positionData = fromRssiToXY(fil.second);
        if (positionData.getX() == -1 || positionData.getY() == -1) continue;
        query.bindValue(":hash", QString::fromStdString(fil.second.begin()->getFcs()));
        query.bindValue(":mac", QString::fromStdString(fil.second.begin()->getMacPeer()));
        query.bindValue(":posx", positionData.getX());
        query.bindValue(":posy", positionData.getY());
        query.bindValue(":timestamp", QDateTime::fromSecsSinceEpoch(fil.second.begin()->getTimestamp()));
        query.bindValue(":ssid", QString::fromStdString(fil.second.begin()->getSsid()));
        //controllo se pacchetto con mac hidden //todo controllare bene
        if((fil.second.begin()->getMacPeer().at(0)>='4' && fil.second.begin()->getMacPeer().at(0)<='7')|| fil.second.begin()->getMacPeer().at(0)>='c'){
            //mac hidden
            query.bindValue(":hidden", 1);
        }else{
            query.bindValue(":hidden", 0);
        }
        if (!query.exec()) {
            qDebug() << query.lastError();
        }
    }
    DEBUG("Ending aggregation")

    // Stampa id pacchetti aggregati rilevati.
    auto clock = std::chrono::system_clock::now();
    std::time_t clock_time = std::chrono::system_clock::to_time_t(clock);
    std::cout << "Situation at " << std::ctime(&clock_time) << std::endl;
    for (auto &fil : map_mac_xy) {
        std::cout << "MAC:" << fil.first << " " << fil.second << std::endl;
    }
    std::cout << "Persone nella stanza: " << map_mac_xy.size() << std::endl;



    // Pulizia deque pacchetti attraverso meccanismo di second chance
    /* Un pacchetto viene eliminato dalla deque solo ed esclusivamente dopo due aggregate. In questo maniera
     * si dovrebbe evitare la possibilità che vengano analizzate ricezioni di pacchetti parziali */
    for (auto it = packets.begin(); it != packets.end();) {
        if (it->second >= 2) {
            it = packets.erase(it);
        } else {
            ++it;
        }
    }

    disconnectDB();
}

bool MonitoringServer::isRunning() {
    return server.isListening();
}

std::deque<Packet> MonitoringServer::getHiddenPackets(uint32_t initTime,uint32_t endTime){
    //query per ottenere i pacchetti con mac hidden nel periodo specificato
    connectDB();

    QDateTime timeInit;
    QDateTime timeEnd;

    timeInit.setTime_t(initTime);   //todo verificare problema fusi, inserisco il timestamp delle 18.00 ma lo trasforma in 20.00. Potrebbe essere un problema per la query
    timeEnd.setTime_t(endTime);



    std::deque<Packet> hiddenPackets;
    QString table="stanza";         //todo vedere da impostazioni
    QSqlQuery query{};
    query.prepare("SELECT * FROM "+ table+ " WHERE hidden='1' AND timestamp>='"+timeInit.toUTC().toString("yyyy-MM-dd hh:mm:ss")+"' AND timestamp<='"+ timeEnd.toUTC().toString("yyyy-MM-dd hh:mm:ss")+"';");
    qDebug()<< query.executedQuery();
    if (!query.exec()) {
        qDebug() << query.lastError();
    }
    QSqlRecord record = query.record();
    if(query.size()==0)
        qDebug() << "Nessun risultato";

    while(query.next()){                                //ciclo su ogni entry selezionata del db
        std::string fcs=query.value(1).toString().toStdString();
        std::string mac=query.value(2).toString().toStdString();
        uint32_t timestamp=query.value(5).toDateTime().toSecsSinceEpoch();
        std::string ssid=query.value(6).toString().toStdString();

        Packet p(-1,fcs,-1,mac,timestamp,ssid);

        double_t posX=query.value(3).toDouble();
        double_t posY=query.value(4).toDouble();
        PositionData positionData(posX,posY);
        p.setPosition(positionData);
        hiddenPackets.push_back(p);

    }
    //todo verificare che funzioni
    return hiddenPackets;
}


/**
* veririca se sono presenti mac simili a source oppure no
* @param source
* @param initTime
* @param endTime
* @return
*/
bool MonitoringServer::getHiddenDeviceFor(Packet source,uint32_t initTime,uint32_t endTime){
    //entro 5 minuti, stessa posizione +-0.5, altro da vedere
    uint32_t tolleranzaTimestamp=240;//usata per definire entro quanto la posizione deve essere uguale, 240= 4 minuti
    double tolleranzaX=0.5;     //todo valutare se ha senso impostare le tolleranze da impostazioni grafiche
    double tolleranzaY=0.5;
    double perc;
    bool trovato= false;

    std::deque<Packet> hiddenPackets=getHiddenPackets(initTime,endTime);
    if(hiddenPackets.size()==0)
        return false;

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
//getHiddenDevice(1569088800,1569091920);
int MonitoringServer::getHiddenDevice(uint32_t initTime,uint32_t endTime){
    bool trovato;
    int numHiddenDevice=0;


    std::deque<Packet> hiddenPackets=getHiddenPackets(initTime,endTime);
    if(hiddenPackets.size()==0)
        return 0;

    for(int i=0;i<hiddenPackets.size();i++){
        trovato=getHiddenDeviceFor(hiddenPackets.at(i),initTime,endTime);
        if(trovato)
            numHiddenDevice++;
    }

    qDebug() << "Numero di dispositivi differenti con mac nascosto: "<< numHiddenDevice;
    return numHiddenDevice;
    //todo decidere cosa fare con tale numero
}






