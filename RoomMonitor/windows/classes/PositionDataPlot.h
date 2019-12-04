#ifndef ROOMMONITOR_POSITIONDATAPLOT_H
#define ROOMMONITOR_POSITIONDATAPLOT_H


#include <QtCore/QDateTime>

class PositionDataPlot {
    /**
     * Time Data
     */
    QDateTime data;

    /**
     * Position x
     */
    qreal x;

    /**
     * Position y
     */
    qreal y;
public:

    /**
     * Costruttore parametrizzato
     * @param data time
     * @param x     posx
     * @param y     posy
     */
    PositionDataPlot(const QDateTime &data, qreal x, qreal y);

    /**
     * Costruttore vuoto
     */
    PositionDataPlot();

    /**
     * Getter Data
     * @return
     */
    const QDateTime &getData() const;

    /**
     * Getter Pos X
     * @return
     */
    qreal getX() const;

    /**
     * Getter Pos Y
     * @return
     */
    qreal getY() const;
};


#endif //ROOMMONITOR_PositionDataPlotPLOT_H
