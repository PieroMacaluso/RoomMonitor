//
// Created by pieromack on 18/09/19.
//

#include "MonitoringPlotView.h"

MonitoringPlotView::MonitoringPlotView(QWidget *parent) : QChartView(parent) {
    this->setMouseTracking(true);
    this->setRubberBand(QChartView::RectangleRubberBand);
}

MonitoringPlotView::MonitoringPlotView(QChart *chart, QWidget *parent) : QChartView(chart, parent) {
    this->setMouseTracking(true);
    this->setRubberBand(QChartView::RectangleRubberBand);
}

void MonitoringPlotView::setChart(MonitoringChart *chart) {
    this->chart = chart;
    QChartView::setChart(chart);
}

void MonitoringPlotView::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->chart->zoomReset();
        this->chart->resetView();
    }

}

void MonitoringPlotView::wheelEvent(QWheelEvent *event) {
    int delta = event->delta() / 10;
    if (event->modifiers().testFlag(Qt::ControlModifier)) {
        this->chart->scroll(delta, 0);
    } else {
        this->chart->scroll(0, delta);
    }
}