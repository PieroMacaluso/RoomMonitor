//
// Created by pieromack on 09/10/19.
//

#include <windows/classes/LastMac.h>
#include "LiveChart.h"

LiveChart::LiveChart(QGraphicsItem *parent, Qt::WindowFlags wFlags) : QChart(parent, wFlags) {
    QScatterSeries *b = new QScatterSeries();
    QScatterSeries *d = new QScatterSeries();
    b->setName("Board");
    d->setName("Device");
    b->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    b->setMarkerSize(24.0);
    d->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    d->setMarkerSize(24.0);
    QImage dev_img = QImage(":/resources/resources/mac.svg");
    QBrush dev_br = QBrush(dev_img);
    d->setBrush(dev_br);

    QImage boa_img = QImage(":/resources/resources/board.svg");
    QBrush boa_br = QBrush(boa_img);
    b->setBrush(boa_br);

    this->setAnimationOptions(QChart::SeriesAnimations);
    this->legend()->hide();
    this->addBoardsSeries(b);
    this->addDevicesSeries(d);


    QValueAxis *axisX = new QValueAxis();
    axisX->setLabelFormat("%i");
    axisX->setTitleText("x");
    this->addX(axisX);
    b->attachAxis(axisX);
    d->attachAxis(axisX);


    QValueAxis *axisY = new QValueAxis();
    axisY->setLabelFormat("%i");
    axisY->setTitleText("y");
    this->addY(axisY);
    b->attachAxis(axisY);
    d->attachAxis(axisY);
    QSettings s{"VALP", "RoomMonitoring"};
    xMax = s.value("room/width").toReal();
    yMax = s.value("room/height").toReal();

    this->legend()->setMarkerShape(QLegend::MarkerShapeFromSeries);
    this->legend()->setVisible(true);
    this->setTitle("Plot Dispositivi in tempo reale");


}

void LiveChart::addBoardsSeries(QScatterSeries *series) {
    this->boards = series;
    QChart::addSeries(series);
}

void LiveChart::addDevicesSeries(QScatterSeries *series) {
    this->devices = series;
    QChart::addSeries(series);
}

void LiveChart::addBoard(qreal xValue, qreal yValue) {
    this->boards->append(xValue, yValue);
}

void LiveChart::addDevice(qreal xValue, qreal yValue) {
    this->devices->append(xValue, yValue);
}


void LiveChart::addX(QValueAxis *axisX) {
    aX = axisX;
    QChart::addAxis(axisX, Qt::AlignBottom);
}

void LiveChart::addY(QValueAxis *axisY) {
    aY = axisY;
    QChart::addAxis(axisY, Qt::AlignLeft);
}

QScatterSeries *LiveChart::getBoardsScatter() {
    return boards;
}

QScatterSeries *LiveChart::getDevicesScatter() {
    return devices;
}

void LiveChart::resetView() {
    aX->setRange(-1, xMax+1);
    aY->setRange(-1, yMax+1);
}

qreal LiveChart::getXMax() const {
    return xMax;
}

qreal LiveChart::getYMax() const {
    return yMax;
}

void LiveChart::fillBoards(std::vector<Board> newData) {
    xMax = 0;
    yMax = 0;
    this->boards_v.clear();
    this->boards_v = std::move(newData);
    this->currentPos = 0;
    for (auto i : this->boards_v){
        this->boards->append(i.getCoord().x(), i.getCoord().y());
        if (i.getCoord().x() > xMax){
            xMax = i.getCoord().x();
        }
        if (i.getCoord().y() > yMax){
            yMax = i.getCoord().y();
        }
    }
}

void LiveChart::fillDevices(QMap<QString, LastMac> &newData) {
    this->devices_v.clear();
    this->currentPos = 0;
    for (auto &i : newData){
        this->devices->append(i.getPosx(), i.getPosy());
        this->devices_v.push_back(i);
    }
}

const std::vector<LastMac> & LiveChart::getDevices() const {
    return devices_v;
}

const std::vector<Board> &LiveChart::getBoards() const {
    return boards_v;
}

