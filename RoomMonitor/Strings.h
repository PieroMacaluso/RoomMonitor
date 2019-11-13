//
// Created by pieromack on 13/11/19.
//

#ifndef ROOMMONITOR_STRINGS_H
#define ROOMMONITOR_STRINGS_H


#include <string>
#include <QtCore/QString>
#include <QtCore>

class Strings: public QObject {
    Q_OBJECT
public:
    static const QString ERR_DB;
    static const QString ERR_DB_MSG;
    static const QString SRV_NOT_STARTED;
    static const QString RANGE_ERROR;
    static const QString RANGE_ERROR_MSG;
    static const QString ANA_RUNNING;
    static const QString ANA_RUNNING_MSG;
    static const QString ANA;
    static const QString MON;
};


#endif //ROOMMONITOR_STRINGS_H
