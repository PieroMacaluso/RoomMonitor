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
#include <jmorecfg.h>
#include "Strings.h"

class Utility {
public:
    static QSqlDatabase getDB(bool &error);
    static QLineSeries * generateRoomSeries(QObject *parent = nullptr);
    static void warningMessage(const QString &title, const QString &text, const QString &error);
    static bool yesNoMessage(QWidget *parent, const QString &title, const QString &text);

    static bool testTable(QSqlDatabase &db);
};


#endif //ROOMMONITOR_UTILITY_H
