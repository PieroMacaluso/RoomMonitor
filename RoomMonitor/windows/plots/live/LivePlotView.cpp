//
// Created by pieromack on 09/10/19.
//

#include "LivePlotView.h"

LivePlotView::LivePlotView(QWidget *parent) : QChartView(parent) {
    this->setMouseTracking(true);
    this->setRubberBand(QChartView::RectangleRubberBand);
    this->setRenderHint(QPainter::Antialiasing);

}

LivePlotView::LivePlotView(QChart *chart, QWidget *parent) : QChartView(chart, parent) {
    this->setMouseTracking(true);
    this->setRubberBand(QChartView::RectangleRubberBand);
    this->setRenderHint(QPainter::Antialiasing);
}

void LivePlotView::setChart(LiveChart *chart) {
    delete m_tooltip;
    m_tooltip = nullptr;
    this->chart = chart;
    connect(this->chart->getDevicesScatter(), &QScatterSeries::hovered, this, &LivePlotView::tooltipDevice);
    connect(this->chart->getBoardsScatter(), &QScatterSeries::hovered, this, &LivePlotView::tooltipBoard);
    this->chart->resetView();
    QChartView::setChart(chart);
}

void LivePlotView::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->chart->zoomReset();
        this->chart->resetView();
    }
}

void LivePlotView::wheelEvent(QWheelEvent *event) {
    int delta = event->delta() / 10;
    if (event->modifiers().testFlag(Qt::ControlModifier)) {
        this->chart->scroll(delta, 0);
    } else {
        this->chart->scroll(0, delta);
    }
}

void LivePlotView::tooltipDevice(QPointF point, bool state) {
    if (m_tooltip == nullptr)
        m_tooltip = new Callout(chart);


    if (state) {
        for (const auto& i : chart->getDevices()){
            if (i.getPosx() == point.x()&& i.getPosy() == point.y()){
                m_tooltip->setText(QString("Device %1 \nPos: (%2, %3)").arg(i.getMac()).arg(point.x()).arg(point.y()));
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

void LivePlotView::tooltipBoard(QPointF point, bool state) {
    if (m_tooltip == nullptr)
        m_tooltip = new Callout(chart);


    if (state) {
        for (const auto& i : chart->getBoards()){
            if (i.getCoord().x() == point.x()&& i.getCoord().y() == point.y()){
                m_tooltip->setText(QString("Board %1 \nPos: (%2, %3)").arg(i.getId()).arg(point.x()).arg(point.y()));
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

void LivePlotView::resizeEvent(QResizeEvent *event) {
    QChartView::resizeEvent(event);
}

LiveChart *LivePlotView::getChart() const {
    return nullptr;
}
