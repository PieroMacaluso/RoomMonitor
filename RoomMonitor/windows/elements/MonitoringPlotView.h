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
    QGraphicsSimpleTextItem *m_coordX;
    QGraphicsSimpleTextItem *m_coordY;
    Callout *m_tooltip = nullptr;
    QList<Callout *> m_callouts;
public:
    explicit MonitoringPlotView(QWidget *parent = nullptr);

    explicit MonitoringPlotView(QChart *chart, QWidget *parent = nullptr);

    void setChart(MonitoringChart *chart);

    void mouseDoubleClickEvent(QMouseEvent *event);

    void wheelEvent(QWheelEvent *event);


//    void mouseMoveEvent(QMouseEvent *event);

    void tooltip(QPointF point, bool state);
};


#endif //ROOMMONITOR_MONITORINGPLOTVIEW_H
