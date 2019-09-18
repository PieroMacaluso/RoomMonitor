//
// Created by pieromack on 17/09/19.
//

#include "Plot1.h"

Plot1::Plot1(QWidget *parent) : QChartView(parent) {
    this->setRenderHint(QPainter::Antialiasing);
}

Plot1::Plot1(QChart *chart, QWidget *parent) : QChartView(chart, parent) {
    this->setRenderHint(QPainter::Antialiasing);
}

void Plot1::wheelEvent(QWheelEvent *event) {
    if ((scroll_to_start <= tick_size * one_line_size && event->delta() > 0) || (scroll_to_start >= -1 && event->delta() < 0)) {
        int delta = one_line_size/2.0;
        if (event->delta()< 0) delta = -delta;
        this->chart()->scroll(0, delta);
        scroll_to_start += delta;
        qDebug() << scroll_to_start;
    }
}

void Plot1::mouseDoubleClickEvent(QMouseEvent *event) {
    this->chart()->zoomReset();
    this->chart()->scroll(0, -scroll_to_start);
    scroll_to_start = 0;
}

void Plot1::setCategorySize(int i) {
    this->tick_size = i-5;

}

void Plot1::resizeEvent(QResizeEvent *event) {
    this->chart()->zoomReset();
    this->chart()->scroll(0, -scroll_to_start);
    scroll_to_start = 0;
    QChartView::resizeEvent(event);
    one_line_size = ceil(this->chart()->plotArea().height() / 5);
    qDebug() << one_line_size;

}
