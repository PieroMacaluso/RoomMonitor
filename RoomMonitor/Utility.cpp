//
// Created by pieromack on 01/10/19.
//

#include "Utility.h"

QSqlDatabase Utility::getDB(bool &error) {
    QSettings su{"VALP", "RoomMonitoring"};
    auto name = "my_db_" + QString::number((quint64) QThread::currentThread(), 16);
    if (QSqlDatabase::contains(name)) {
        qDebug() << "Not Contain";
        return QSqlDatabase::database(name);
    } else {
        qDebug() << "Contain";
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
    QSettings su{"VALP", "RoomMonitoring"};
    QSqlQuery query{db};
    query.prepare("SELECT * FROM " + su.value("database/table").toString() + ";");
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
    QSettings su{"VALP", "RoomMonitoring"};
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
    reply = QMessageBox::question(parent, Strings::ANA_RUNNING, Strings::ANA_RUNNING_MSG,
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        qDebug() << "Yes was clicked";
        return true;
    }
    qDebug() << "Yes was *not* clicked";
    return false;
}
