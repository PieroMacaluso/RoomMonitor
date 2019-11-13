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
};


#endif //ROOMMONITOR_QUERY_H
