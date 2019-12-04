#ifndef ROOMMONITOR_LASTMAC_H
#define ROOMMONITOR_LASTMAC_H


#include <QtCore/QDateTime>

class LastMac {
    /**
     * MAC string
     */
    QString mac;

    /**
     * Date of the last position Available
     */
    QDateTime timing;
    /**
     * Position X
     */
    qreal posx;
    /**
     * Position Y
     */
    qreal posy;

public:
    /**
     * Empty constructor
     */
    LastMac();

    /**
     * Parametrized Constructor
     * @param mac
     * @param timing
     * @param posx
     * @param posy
     */
    LastMac(const QString &mac, const QDateTime &timing, qreal posx, qreal posy);

    /**
     * Update the LastMac object
     * @param timing
     * @param posx
     * @param posy
     */
    void update(const QDateTime &timing, qreal posx, qreal posy);

    /**
     * Getter MAC
     * @return
     */
    const QString &getMac() const;

    /**
     * Getter Timestamp
     * @return
     */
    const QDateTime &getTiming() const;

    /**
     * Get Position X
     * @return
     */
    qreal getPosx() const;

    /**
     * Get Position Y
     * @return
     */
    qreal getPosy() const;

};


#endif //ROOMMONITOR_LASTMAC_H
