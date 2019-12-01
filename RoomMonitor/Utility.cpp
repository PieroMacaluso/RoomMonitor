#include "Utility.h"

const QString Utility::ORGANIZATION = "ALP";
const QString Utility::APPLICATION = "RoomMonitor";
int Utility::RETRY_STEP_BOARD = 3;
QString Utility::LOG_DIR = "log";
QString Utility::LOG_FILE = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");

QSqlDatabase Utility::getDB(bool &error) {
    QSettings su{Utility::ORGANIZATION, Utility::APPLICATION};
    auto name = "my_db_" + QString::number((quint64) QThread::currentThread(), 16);
    if (QSqlDatabase::contains(name)) {
        return QSqlDatabase::database(name);
    } else {
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", name);
        db.setHostName(su.value("database/host").toString());
        db.setDatabaseName(su.value("database/name").toString());
        db.setPort(su.value("database/port").toInt());
        db.setUserName(su.value("database/user").toString());
        db.setPassword(su.value("database/pass").toString());
        if (!db.open()) {
            //qDebug() << db.lastError();
            Utility::warningMessage(Strings::ERR_DB,
                                    Strings::ERR_DB_MSG,
                                    db.lastError().text());
            error = true;
        }
        return db;
    }
}

bool Utility::testTable(QSqlDatabase &db) {
    QSettings su{Utility::ORGANIZATION, Utility::APPLICATION};
    QSqlQuery query{db};
    query.prepare(Query::SELECT_ALL_PACKET.arg(su.value("database/table").toString()));
    if (!query.exec()) {
        //qDebug() << query.lastError();
        Utility::warningMessage(Strings::ERR_DB,
                                Strings::ERR_DB_MSG,
                                query.lastError().text());
        db.close();
        return false;
    }
    query.prepare(Query::SELECT_ALL_BOARD.arg(su.value("database/table").toString()));
    if (!query.exec()) {
        //qDebug() << query.lastError();
        Utility::warningMessage(Strings::ERR_DB,
                                Strings::ERR_DB_MSG,
                                query.lastError().text());
        db.close();
        return false;
    }
    return true;
}

QLineSeries *Utility::generateRoomSeries(QObject *parent) {
    QSettings su{Utility::ORGANIZATION, Utility::APPLICATION};
    QLineSeries *series = new QLineSeries(parent);
    series->setColor(QColor("green"));
    series->append(0, 0);
    series->append(su.value("room/width").toInt(), 0);
    series->append(su.value("room/width").toInt(), su.value("room/height").toInt());
    series->append(0, su.value("room/height").toInt());
    series->append(0, 0);
    series->setName("Perimeter");
    return series;
}

void Utility::warningMessage(const QString &title, const QString &text, const QString &error) {
    QMessageBox m{};
    m.setStandardButtons(QMessageBox::Close);
    m.setWindowTitle(title);
    m.setIcon(QMessageBox::Warning);
    m.setText(text);
    m.setDetailedText(error);
    m.exec();

    if (text == error) {
        qCritical() << text;
    } else {
        qCritical() << text << "," << error;
    }

}

bool Utility::yesNoMessage(QWidget *parent, const QString &title, const QString &text) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(parent, title, text,
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        qDebug() << "Yes was clicked";
        return true;
    }
    qDebug() << "Yes was *not* clicked";
    return false;
}

std::vector<Board> Utility::getBoards() {
    std::vector<Board> res;
    QSettings su{Utility::ORGANIZATION, Utility::APPLICATION};
    bool error = false;
    QSqlDatabase db = Utility::getDB(error);
    if (error) exit(-1);

    QSqlQuery query{db};
    query.prepare(Query::SELECT_ALL_BOARD.arg(su.value("database/table").toString()));
    if (!query.exec()) {
        Utility::warningMessage(Strings::ERR_DB, Strings::ERR_DB_MSG, query.lastError().text());
        exit(-1);
    }
    while (query.next()) {
        int id = query.value(0).toInt();
        qreal posx = query.value(1).toDouble();
        qreal posy = query.value(2).toDouble();
        int A = query.value(3).toInt();
        Board b{id, posx, posy, A};
        res.push_back(b);
    }
    db.close();
    return res;
}


void Utility::dropBoards() {
    std::vector<Board> res;
    QSettings su{Utility::ORGANIZATION, Utility::APPLICATION};
    bool error = false;
    QSqlDatabase db = Utility::getDB(error);
    if (error) exit(-1);

    QSqlQuery query{db};
    query.prepare(Query::DROP_TABLE_BOARD.arg(su.value("database/table").toString()));
    if (!query.exec()) {
        Utility::warningMessage(Strings::ERR_DB, Strings::ERR_DB_MSG, query.lastError().text());
        exit(-1);
    }
    query.prepare(Query::CREATE_TABLE_BOARD.arg(su.value("database/table").toString()));
    if (!query.exec()) {
        Utility::warningMessage(Strings::ERR_DB, Strings::ERR_DB_MSG, query.lastError().text());
        exit(-1);
    }
    db.close();
}

void Utility::infoMessage(const QString &title, const QString &text) {
    QMessageBox m{};
    m.setStandardButtons(QMessageBox::Ok);
    m.setWindowTitle(title);
    m.setIcon(QMessageBox::Information);
    m.setText(text);
    m.exec();

    qInfo() << text;
}

int Utility::infoMessageTimer(const QString &title, const QString &text, int millisec) {
    QMessageBox m{};
    m.setWindowTitle(title);
    m.setStandardButtons(QMessageBox::Ok | QMessageBox::Abort);
    m.setText(text);
    QTimer t{};
    t.setInterval(millisec);
    t.setSingleShot(true);
    QObject::connect(&m, &QMessageBox::rejected, &t, &QTimer::stop);
    QObject::connect(&t, &QTimer::timeout, m.button(QMessageBox::Ok), &QAbstractButton::click);
    t.start();
    return m.exec();
}

void Utility::setupVariables() {
    QSettings su{Utility::ORGANIZATION, Utility::APPLICATION};
    RETRY_STEP_BOARD = su.value("Utility/RETRY_STEP_BOARD").toInt();
}


std::deque<Packet> Utility::string2packet(const std::vector<std::string> &p, const QList<int>& l) {
    std::deque<Packet> deque;
    std::string ssid;

    for (const std::string &s:p) {
        std::vector<std::string> packet_hmac;
        std::vector<std::string> values;
        // Esempio: "2,8e13f31f,-69,b4:f1:da:d9:2b:b2,1xxxxxxxxxxxxxxxxxxxxxxxxxxxxx" ->
            //          "2,dc7ef681,-76,b4:f1:da:d9:2b:b2,1573214966,~,3C:71:BF:F5:9F:3C"
            Utility::split(s, packet_hmac, '|');
        if (packet_hmac.size() != 2) {
            qCritical() << "Stringa non ricevuta correttemente: " << QString::fromStdString(s);
            continue;
        }
        Utility::split(packet_hmac[0], values, ',');
        if (values.size() != 7) {
            qCritical() << "Stringa non ricevuta correttemente: " << QString::fromStdString(s);
            continue;
        }
        QMessageAuthenticationCode code(QCryptographicHash::Sha256);
        QSettings su{Utility::ORGANIZATION, Utility::APPLICATION};
        code.setKey(su.value("secret").toByteArray());
        packet_hmac[0] = packet_hmac[0] + '|';

        QByteArray byteArray(packet_hmac[0].c_str(), packet_hmac[0].length());
        code.addData(byteArray);
        if (code.result().toHex() == QString::fromStdString(packet_hmac[1])) {
            // Alcuni pacchetti contengono SSID, altri no
            if (values[5] == "~")
                ssid = "Nan";
            else
                ssid = values[5];
            if (!l.contains(std::stoi(values[0]))) throw std::invalid_argument{"Board non configurata"};


            Packet packet(std::stoi(values[0]), values[1], std::stoi(values[2]), values[3],
                          std::stoi(values[4]), ssid);
            deque.push_back(packet);
        } else {
            qWarning() << "Pacchetto con Hash non coincidente " << QString::fromStdString(s);
            throw std::invalid_argument{"Pacchetto corrotto o board non autorizzata"};
        }
    }
    return deque;
}

template<class Container>
void Utility::split(const std::string &str, Container &cont, char delim) {
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim)) {
        cont.push_back(token);
    }
}
