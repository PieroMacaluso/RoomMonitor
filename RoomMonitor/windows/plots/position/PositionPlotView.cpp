//
// Created by pieromack on 22/09/19.
//

#include "PositionPlotView.h"

PositionPlotView::PositionPlotView(QWidget *parent) : QChartView(parent) {
    this->setMouseTracking(true);
    this->setRubberBand(QChartView::RectangleRubberBand);
    this->setRenderHint(QPainter::Antialiasing);
}

PositionPlotView::PositionPlotView(QChart *chart, QWidget *parent) : QChartView(chart, parent) {
    this->setMouseTracking(true);
    this->setRubberBand(QChartView::RectangleRubberBand);
    this->setRenderHint(QPainter::Antialiasing);
}

void PositionPlotView::setChart(PositionPlot *chart) {
    delete m_tooltip;
    m_tooltip = nullptr;
    this->chart = chart;
    QChartView::setChart(chart);
    connect(this->chart->getScatter(), &QScatterSeries::hovered, this, &PositionPlotView::tooltip);
    this->chart->resetView();
}

void PositionPlotView::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->chart->zoomReset();
        this->chart->resetView();
    }
}

void PositionPlotView::wheelEvent(QWheelEvent *event) {
    int delta = event->delta() / 10;
    if (event->modifiers().testFlag(Qt::ControlModifier)) {
        this->chart->scroll(delta, 0);
    } else {
        this->chart->scroll(0, delta);
    }
}

void PositionPlotView::tooltip(QPointF point, bool state) {
    if (m_tooltip == nullptr)
        m_tooltip = new Callout(chart);


    if (state) {
        for (const auto& i : chart->getData1()){
            if (i.getX() == point.x()&& i.getY() == point.y()){
                m_tooltip->setText(QString("Time: %1 \nPos: (%2, %3)").arg(i.getData().toString("dd/MM/yy hh:mm:ss")).arg(point.x()).arg(point.y()));
                m_tooltip->setAnchor(point);
                m_tooltip->setZValue(11);
                m_tooltip->updateGeometry();
                m_tooltip->show();
                break;
            }
        }

    } else {
        m_tooltip->hide();
    }
}
