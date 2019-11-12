//
// Created by pieromack on 01/10/19.
//

#ifndef ROOMMONITOR_UTILITY_H
#define ROOMMONITOR_UTILITY_H


#include <QtSql>
#include <QtCore/QSettings>
#include <QDebug>
#include <QThread>
#include <QtCharts>

class Utility {
public:
    static QSqlDatabase getDB();
    static QLineSeries * generateRoomSeries(QObject *parent = nullptr);

};


#endif //ROOMMONITOR_UTILITY_H
