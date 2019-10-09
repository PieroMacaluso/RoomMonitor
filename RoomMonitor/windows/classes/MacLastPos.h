//
// Created by pieromack on 10/10/19.
//

#ifndef ROOMMONITOR_MACLASTPOS_H
#define ROOMMONITOR_MACLASTPOS_H


#include <QtCore/QtGlobal>
#include <QString>


class MacLastPos {
    QString mac;
    qreal x;
    qreal y;
public:
    MacLastPos(const QString &mac, qreal x, qreal y);

    const QString &getMac() const;

    qreal getX() const;

    qreal getY() const;


};


#endif //ROOMMONITOR_MACLASTPOS_H
