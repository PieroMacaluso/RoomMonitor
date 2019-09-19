//
// Created by pieromack on 18/09/19.
//

#ifndef ROOMMONITOR_MONITORINGPLOTVIEW_H
#define ROOMMONITOR_MONITORINGPLOTVIEW_H

#include <QtCharts>
#include "MonitoringChart.h"
#include "callout.h"


class MonitoringPlotView : public QChartView {
    MonitoringChart * chart;
    Callout *m_tooltip = nullptr;
public:
    explicit MonitoringPlotView(QWidget *parent = nullptr);

    explicit MonitoringPlotView(QChart *chart, QWidget *parent = nullptr);

    void setChart(MonitoringChart *chart);

    void mouseDoubleClickEvent(QMouseEvent *event);

    void wheelEvent(QWheelEvent *event);

    void tooltip(QPointF point, bool state);
};


#endif //ROOMMONITOR_MONITORINGPLOTVIEW_H
