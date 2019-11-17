//
// Created by pieromack on 17/11/19.
//

#ifndef ROOMMONITOR_STATISTIC_H
#define ROOMMONITOR_STATISTIC_H


#include <monitoring/Packet.h>
#include <QtCore>

class Statistic {
    QString mac;
    qreal perc;

public:
    Statistic(const QString &mac, qreal perc);

    const QString &getMac() const;

    qreal getPerc() const;

};


#endif //ROOMMONITOR_STATISTIC_H
