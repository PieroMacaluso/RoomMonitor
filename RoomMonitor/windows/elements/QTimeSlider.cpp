#include "QTimeSlider.h"
#include <QDebug>
#include <QSlider>
#include <QtWidgets/QLabel>

void QTimeSlider::mouseMoveEvent(QMouseEvent *ev) {
    emitDateMax(data[this->value()]);
    QSlider::mouseMoveEvent(ev);
}

QTimeSlider::QTimeSlider(QWidget *parent) : QSlider(parent) {
}

void QTimeSlider::setData(const std::vector<PositionDataPlot> &data) {
    QTimeSlider::data = data;
}