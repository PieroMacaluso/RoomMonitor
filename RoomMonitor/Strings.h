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
    static const QString SET_DEF;
    static const QString SET_DEF_MSG;
    static const QString SET_DEF_LOG;
    static const QString RESTORE_BOARD;
    static const QString RESTORE_BOARD_MSG;
    static const QString SRV_STARTED;
    static const QString RM_STARTED;
    static const QString RM_STOPPED;
    static const QString SRV_STOPPED;
    static const QString AGG_STARTED;
    static const QString AGG_STOPPED;
    static const QString DB_RESET;
    static const QString SET_CONF;
    static const QString NOT_WORKING_BOARD;
    static const QString NOT_WORKING_BOARD_MSG;
};


#endif //ROOMMONITOR_STRINGS_H
