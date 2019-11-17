//
// Created by pieromack on 13/11/19.
//

#ifndef ROOMMONITOR_QUERY_H
#define ROOMMONITOR_QUERY_H


#include <QtCore/QString>

class Query {
public:
    static const QString DROP_TABLE_PACKET;
    static const QString DROP_TABLE_BOARD;
    static const QString CREATE_TABLE_PACKET;
    static const QString CREATE_TABLE_BOARD;
    static const QString SELECT_ALL_BOARD;
    static const QString INSERT_BOARD;
    static const QString SELECT_ALL_PACKET;
    static const QString SELECT_TIMING_COUNT_BUCKET_FREQ;
    static const QString SELECT_MAC_COUNT_HIDDEN_ORDERBY_FREQ;
    static const QString SELECT_MAC_COUNT_HIDDEN_ORDERBY_MAC;
    static const QString SELECT_MAC_TIMING_AVGPOS;
    static const QString SELECT_MAC_TIMING_AVGPOS_SINGLE;
    static const QString SELECT_COUNT_LIVE;
    static const QString SELECT_MAC_TIMING_LASTPOS;
    static const QString SELECT_HIDDEN_NOT_MAC;
    static const QString SELECT_HIDDEN_MAC;
};


#endif //ROOMMONITOR_QUERY_H
