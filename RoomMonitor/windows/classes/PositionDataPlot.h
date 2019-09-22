//
// Created by pieromack on 22/09/19.
//

#ifndef ROOMMONITOR_POSITIONDATAPLOT_H
#define ROOMMONITOR_POSITIONDATAPLOT_H


#include <QtCore/QDateTime>

class PositionDataPlot {
    QDateTime data;
    qreal x;
    qreal y;
public:
    PositionDataPlot(const QDateTime &data, qreal x, qreal y);

    PositionDataPlot();

    const QDateTime &getData() const;

    qreal getX() const;

    qreal getY() const;
};


#endif //ROOMMONITOR_PositionDataPlotPLOT_H
