//
// Created by pieromack on 18/09/19.
//

#include "MonitoringPlotView.h"

MonitoringPlotView::MonitoringPlotView(QWidget *parent) : QChartView(parent) {
    this->setMouseTracking(true);
    this->setRubberBand(QChartView::RectangleRubberBand);
    this->setRenderHint(QPainter::Antialiasing);
}

MonitoringPlotView::MonitoringPlotView(QChart *chart, QWidget *parent) : QChartView(chart, parent) {
    this->setMouseTracking(true);
    this->setRubberBand(QChartView::RectangleRubberBand);
    this->setRenderHint(QPainter::Antialiasing);

}

void MonitoringPlotView::setChart(MonitoringChart *chart) {
    this->chart = chart;
    QChartView::setChart(chart);
    connect(this->chart->getScatter(), &QScatterSeries::hovered, this, &MonitoringPlotView::tooltip);
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

void MonitoringPlotView::tooltip(QPointF point, bool state)
{
    if (m_tooltip == nullptr)
        m_tooltip = new Callout(chart);

    if (state) {
        QDateTime s{};
        s.setMSecsSinceEpoch(point.x());
        m_tooltip->setText(QString("%1 \nDispositivi: %2 ").arg(s.toString("dd/MM/yy hh:mm:ss")).arg(point.y()));
        m_tooltip->setAnchor(point);
        m_tooltip->setZValue(11);
        m_tooltip->updateGeometry();
        m_tooltip->show();
    } else {
        m_tooltip->hide();

    }
}

MonitoringChart *MonitoringPlotView::getChart() const {
    return chart;
}
