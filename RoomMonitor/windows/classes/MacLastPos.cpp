//
// Created by pieromack on 10/10/19.
//

#include "MacLastPos.h"

MacLastPos::MacLastPos(const QString &mac, qreal x, qreal y) : mac(mac), x(x), y(y) {}

const QString &MacLastPos::getMac() const {
    return mac;
}

qreal MacLastPos::getX() const {
    return x;
}

qreal MacLastPos::getY() const {
    return y;
}
