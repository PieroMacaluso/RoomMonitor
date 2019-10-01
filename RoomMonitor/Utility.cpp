//
// Created by pieromack on 01/10/19.
//

#include "Utility.h"

QSqlDatabase Utility::getDB() {
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
        if (!db.open())
            qDebug() << db.lastError();
        return db;
    }
}
