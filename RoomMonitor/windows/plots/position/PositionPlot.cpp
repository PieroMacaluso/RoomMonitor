//
// Created by pieromack on 22/09/19.
//

#include <monitoring/PositionData.h>

#include <utility>
#include <Utility.h>
#include "PositionPlot.h"

PositionPlot::PositionPlot(QGraphicsItem *parent, Qt::WindowFlags wFlags) : QChart(parent, wFlags) {
    QLineSeries* roomPerimeter = Utility::generateRoomSeries(this);
    QLineSeries *ls = new QLineSeries();
    QScatterSeries *scatter = new QScatterSeries();
    scatter->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    scatter->setMarkerSize(15.0);

    this->setAnimationOptions(QChart::SeriesAnimations);
    this->legend()->hide();
    this->addSeries(ls);
    this->addSeries(scatter);
    QChart::addSeries(roomPerimeter);


    QValueAxis *axisX = new QValueAxis;
    axisX->setLabelFormat("%i");
    axisX->setTitleText("X");
    this->addX(axisX);
    lineSeries->attachAxis(axisX);
    scatter->attachAxis(axisX);
    roomPerimeter->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setLabelFormat("%i");
    axisY->setTitleText("Y");
    this->addY(axisY);
    lineSeries->attachAxis(axisY);
    scatter->attachAxis(axisY);
    roomPerimeter->attachAxis(axisY);
    QSettings s{Utility::ORGANIZATION, Utility::APPLICATION};
    xMax = s.value("room/width").toReal();
    yMax = s.value("room/height").toReal();
}

void PositionPlot::addSeries(QLineSeries *series) {
    this->lineSeries = series;
    QChart::addSeries(series);

}

void PositionPlot::addData(qreal xValue, qreal yValue) {
    this->lineSeries->append(xValue, yValue);
    this->scatter->append(xValue, yValue);

}

void PositionPlot::addX(QValueAxis *axisX) {
    aX = axisX;
    QChart::addAxis(axisX, Qt::AlignBottom);
}

void PositionPlot::addY(QValueAxis *axisY) {
    aY = axisY;
    QChart::addAxis(axisY, Qt::AlignLeft);
}

QLineSeries *PositionPlot::getLineSeries() {
    return lineSeries;
}

QScatterSeries *PositionPlot::getScatter() {
    return scatter;
}

void PositionPlot::resetView() {
    aX->setRange(-1, xMax+1);
    aY->setRange(-1, yMax+1);
}

void PositionPlot::addSeries(QScatterSeries *series) {
    this->scatter = series;
    QChart::addSeries(series);
}

qreal PositionPlot::getXMax() const {
    return xMax;
}

qreal PositionPlot::getYMax() const {
    return yMax;
}

void PositionPlot::fillData(std::vector<PositionDataPlot> newData) {
    this->data.clear();
    this->data = std::move(newData);
    this->currentPos = 0;
    this->lineSeries->append(data[0].getX(), data[0].getY());
    this->scatter->append(data[0].getX(), data[0].getY());
}

void PositionPlot::sliderChanged(int position){
    if (currentPos < position){
        for (int i = currentPos + 1; i <= position; i++) {
            this->lineSeries->append(data[i].getX(), data[i].getY());
            this->scatter->append(data[i].getX(), data[i].getY());
        }
    } else if (currentPos > position){
        for (int i = currentPos; i > position; i--) {
            this->lineSeries->remove(data[i].getX(), data[i].getY());
            this->scatter->remove(data[i].getX(), data[i].getY());
        }
    }
    currentPos = position;
    emit dataChanged(data[currentPos]);
}

const PositionDataPlot &PositionPlot::getData(int index) const {
    return data[index];
}

const std::vector<PositionDataPlot> &PositionPlot::getData1() const {
    return data;
}
