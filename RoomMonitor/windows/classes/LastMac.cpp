//
// Created by pieromack on 27/09/19.
//

#include "LastMac.h"

LastMac::LastMac() {}

LastMac::LastMac(const QString &mac, const QDateTime &timing, qreal posx, qreal posy) : mac(mac), timing(timing),
                                                                                        posx(posx), posy(posy) {}

void LastMac::update(const QDateTime &timing, qreal posx, qreal posy) {
    this->timing = timing;
    this->posx = posx;
    this->posy = posy;
}

const QString &LastMac::getMac() const {
    return mac;
}

const QDateTime &LastMac::getTiming() const {
    return timing;
}

qreal LastMac::getPosx() const {
    return posx;
}

qreal LastMac::getPosy() const {
    return posy;
}

