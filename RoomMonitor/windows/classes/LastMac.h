//
// Created by pieromack on 27/09/19.
//

#ifndef ROOMMONITOR_LASTMAC_H
#define ROOMMONITOR_LASTMAC_H


#include <QtCore/QDateTime>

class LastMac {
    QString mac;
    QDateTime timing;
    qreal posx;
    qreal posy;

public:
    LastMac();

    LastMac(const QString &mac, const QDateTime &timing, qreal posx, qreal posy);

    void update(const QDateTime &timing, qreal posx, qreal posy);

    const QString &getMac() const;

    const QDateTime &getTiming() const;

    qreal getPosx() const;

    qreal getPosy() const;

};


#endif //ROOMMONITOR_LASTMAC_H
