//
// Created by pieromack on 22/09/19.
//

#include "MacPlotView.h"

MacPlotView::MacPlotView(QWidget *parent) : QChartView(parent) {
    this->setMouseTracking(true);
    this->setRubberBand(QChartView::VerticalRubberBand);
    this->setRenderHint(QPainter::Antialiasing);
}

MacPlotView::MacPlotView(QChart *chart, QWidget *parent) : QChartView(chart, parent) {
    this->setMouseTracking(true);
    this->setRubberBand(QChartView::VerticalRubberBand);
    this->setRenderHint(QPainter::Antialiasing);

}

void MacPlotView::setChart(MacChart *chart) {
    delete m_tooltip;
    m_tooltip = nullptr;
    this->chart = chart;
    connect(this->chart->getBarSeries(), &QHorizontalBarSeries::hovered, this, &MacPlotView::tooltip);
    int mac = floor(this->height() / 50.0);
    this->chart->resetView(mac);
    QChartView::setChart(chart);
}

void MacPlotView::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        int mac = floor(this->height() / 50);
//        this->positionPlot->zoomReset();
        this->chart->resetView(mac);
    }

}

void MacPlotView::wheelEvent(QWheelEvent *event) {
    if (!this->chart->isZoomed()) {
        this->chart->zoomIn();
        this->chart->zoomOut();
    }
    int delta = event->delta() / 10;
    this->chart->scroll(0, delta);
}

void MacPlotView::tooltip(bool state, int index, QBarSet *barset) {
    if (m_tooltip == nullptr)
        m_tooltip = new Callout(chart);

    if (state) {
        QDateTime s{};
        barset->at(index);
        m_tooltip->setText(QString(MacPlotView::calloutText).arg(this->chart->getMacs()[index]).arg(barset->at(index)));
        m_tooltip->setAnchor(QPointF(barset->at(index), index));
        m_tooltip->setZValue(11);
        m_tooltip->updateGeometry();
        m_tooltip->show();
    } else {
        m_tooltip->hide();
    }
}

void MacPlotView::resizeEvent(QResizeEvent *event) {
    int mac = floor(this->height() / 50);
//        this->positionPlot->zoomReset();
    if (this->chart != nullptr)
        this->chart->resetView(mac);
    QChartView::resizeEvent(event);
}

MacChart *MacPlotView::getChart() const {
    return chart;
}

void MacPlotView::setCalloutText(const QString &calloutText) {
    MacPlotView::calloutText = calloutText;
}
