#include "Statistic.h"

Statistic::Statistic(const QString &mac, qreal perc) : mac(mac), perc(perc) {}

const QString &Statistic::getMac() const {
    return mac;
}

qreal Statistic::getPerc() const {
    return perc;
}
