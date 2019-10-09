//
// Created by pieromack on 09/10/19.
//

#ifndef ROOMMONITOR_LIVEPLOTVIEW_H
#define ROOMMONITOR_LIVEPLOTVIEW_H

#include <QtCharts>
#include <windows/common/callout.h>
#include "LiveChart.h"


class LivePlotView : public QChartView {
    LiveChart * chart = nullptr;
    Callout *m_tooltip = nullptr;

public:
    explicit LivePlotView(QWidget *parent = nullptr);

    explicit LivePlotView(QChart *chart, QWidget *parent = nullptr);

    void setChart(LiveChart *chart);

    void mouseDoubleClickEvent(QMouseEvent *event);

    void wheelEvent(QWheelEvent *event);

    void tooltipBoard(QPointF point, bool state);
    void tooltipDevice(QPointF point, bool state);


    void resizeEvent(QResizeEvent *event);

    LiveChart *getChart() const;

};


#endif //ROOMMONITOR_LIVEPLOTVIEW_H
