//
// Created by pieromack on 22/09/19.
//

#include "PositionDataPlot.h"

PositionDataPlot::PositionDataPlot(const QDateTime &data, qreal x, qreal y) : data(data), x(x), y(y) {}

const QDateTime &PositionDataPlot::getData() const {
    return data;
}

qreal PositionDataPlot::getX() const {
    return x;
}

qreal PositionDataPlot::getY() const {
    return y;
}

PositionDataPlot::PositionDataPlot() {}
