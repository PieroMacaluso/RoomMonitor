//
// Created by pieromack on 01/10/19.
//

#include "Utility.h"

const QString Utility::ORGANIZATION = "VALP";
const QString Utility::APPLICATION = "RoomMonitoring";
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
            qDebug() << db.lastError();
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
        qDebug() << query.lastError();
        Utility::warningMessage(Strings::ERR_DB,
                                Strings::ERR_DB_MSG,
                                query.lastError().text());
        db.close();
        return false;
    }
    query.prepare(Query::SELECT_ALL_BOARD.arg(su.value("database/table").toString()));
    if (!query.exec()) {
        qDebug() << query.lastError();
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


