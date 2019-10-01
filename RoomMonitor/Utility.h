//
// Created by pieromack on 01/10/19.
//

#ifndef ROOMMONITOR_UTILITY_H
#define ROOMMONITOR_UTILITY_H


#include <QtSql>
#include <QtCore/QSettings>
#include <QDebug>
#include <QThread>

class Utility {
public:
    static QSqlDatabase getDB();

};


#endif //ROOMMONITOR_UTILITY_H
