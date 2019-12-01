#ifndef ROOMMONITOR_STATISTIC_H
#define ROOMMONITOR_STATISTIC_H


#include <monitoring/Packet.h>
#include <QtCore>

class Statistic {
    /**
     * MAC
     */
    QString mac;

    /**
     * Percentuale di somiglianza MAC Nascosto
     */
    qreal perc;

public:
    /**
     * Costruttore statistica
     * @param mac       MAC
     * @param perc      percentuale di somiglianza MAC nascosto
     */
    Statistic(const QString &mac, qreal perc);

    /**
     * Getter Max
     * @return QString  MAC
     */
    const QString &getMac() const;

    /**
     * Getter Percentuale
     * @return qreal    percentuale di somiglianza MAC nascosto
     */
    qreal getPerc() const;

};

/**
 * Operatore di comparazione
 * @param e1    Statistic   primo elemento nella comparazione
 * @param e2    Statistic   secondo elemento nella comparazione
 * @return      true se e1 > e2, false altrimenti
 */
inline bool operator<(const Statistic &e1, const Statistic &e2)
{
    if (e1.getPerc() != e2.getPerc())
        return e1.getPerc() > e2.getPerc();
    return e1.getMac() > e2.getMac();
}


#endif //ROOMMONITOR_STATISTIC_H
