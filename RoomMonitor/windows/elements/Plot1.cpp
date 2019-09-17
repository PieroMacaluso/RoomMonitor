//
// Created by pieromack on 17/09/19.
//

#include "Plot1.h"

Plot1::Plot1(QWidget *parent) : QChartView(parent) {
    qDebug() << "OU ";

    this->setRenderHint(QPainter::Antialiasing);
    this->setRubberBand(QChartView::VerticalRubberBand);
}

Plot1::Plot1(QChart *chart, QWidget *parent) : QChartView(chart, parent) {
    qDebug() << "OU ";

    this->setRenderHint(QPainter::Antialiasing);
    this->setRubberBand(QChartView::VerticalRubberBand);
}

void Plot1::wheelEvent(QWheelEvent *event) {
    QBarCategoryAxis a{this->chart()->axes(Qt::Vertical).first()};
//    if (a.min() > nullptr) {
////        if (this->chart()->scenePos()) {
//    }
//    qDebug() <<a.visi;
    this->chart()->scroll(0, event->delta() / 10);
}

void Plot1::mouseDoubleClickEvent(QMouseEvent *event) {
    this->chart()->zoomReset();
    this->chart()->mapToPosition(QPointF(0,0));
//    this->chart()->rese(0, 0);
}
