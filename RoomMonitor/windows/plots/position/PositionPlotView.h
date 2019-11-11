//
// Created by pieromack on 22/09/19.
//

#ifndef ROOMMONITOR_POSITIONPLOTVIEW_H
#define ROOMMONITOR_POSITIONPLOTVIEW_H

#include <QtCharts>
#include "PositionPlot.h"
#include "windows/common/callout.h"

class PositionPlotView : public QChartView {
    PositionPlot * positionPlot;
    Callout *m_tooltip = nullptr;
public:
    explicit PositionPlotView(QWidget *parent = nullptr);

    explicit PositionPlotView(QChart *chart, QWidget *parent = nullptr);

    void setChart(PositionPlot *chart);

    void mouseDoubleClickEvent(QMouseEvent *event);

    void wheelEvent(QWheelEvent *event);

    void tooltip(QPointF point, bool state);
};


#endif //ROOMMONITOR_POSITIONPLOTVIEW_H
