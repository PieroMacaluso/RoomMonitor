//
// Created by pieromack on 18/09/19.
//

#include "MonitoringChart.h"
#include "MonitoringPlotView.h"

void MonitoringChart::addSeries(QLineSeries *series) {
    this->lineSeries = series;
    connect(lineSeries, &QLineSeries::pointAdded, [=](int index) {
        qreal y = lineSeries->at(index).y();

        if (y < yMin || y > yMax) {
            if (y < yMin)
                yMin = y;
            if (y > yMax)
                yMax = y;
            aY->setRange(yMin - 2, yMax + 2);
        }

        qreal x = lineSeries->at(index).x();

        if ((x < xMin || xMin == 0) || (x > xMax || xMax == 0)) {
            if (xMin == 0 && xMax == 0) {
                xMin = x;
                xMax = x;
            } else {
                if (x < xMin)
                    xMin = x;
                if (x > xMax)
                    xMax = x;
            }
            QDateTime s;
            QDateTime e;
            s.setMSecsSinceEpoch(xMin - 1000 * 60);
            e.setMSecsSinceEpoch(xMax + 1000 * 60);
            aX->setRange(s, e);
        }

    });

    connect(lineSeries, &QLineSeries::pointReplaced, [=](int index) {
        qDebug() <<"Replace";
        qreal y = lineSeries->at(index).y();

        if (y < yMin || y > yMax) {
            if (y < yMin)
                yMin = y;
            if (y > yMax)
                yMax = y;
            aY->setRange(yMin - 2, yMax + 2);
        }

        qreal x = lineSeries->at(index).x();

        if ((x < xMin || xMin == 0) || (x > xMax || xMax == 0)) {
            if (xMin == 0 && xMax == 0) {
                xMin = x;
                xMax = x;
            } else {
                if (x < xMin)
                    xMin = x;
                if (x > xMax)
                    xMax = x;
            }
            QDateTime s;
            QDateTime e;
            s.setMSecsSinceEpoch(xMin - 1000 * 60);
            e.setMSecsSinceEpoch(xMax + 1000 * 60);
            aX->setRange(s, e);
        }

    });
    QChart::addSeries(series);
}

void MonitoringChart::addSeries(QScatterSeries *series) {
    this->scatter = series;
    QChart::addSeries(series);
}

MonitoringChart::~MonitoringChart() {
    free(lineSeries);
    free(aX);
    free(aY);

}

void MonitoringChart::addData(QDateTime time, int value) {
    this->lineSeries->append(time.toMSecsSinceEpoch(), value);
    this->scatter->append(time.toMSecsSinceEpoch(), value);

}

void MonitoringChart::updateData(QDateTime time, int value) {
    for (int i = 0; i < lineSeries->count(); i++) {
        auto point = this->lineSeries->at(i);
        if (point.x() == time.toMSecsSinceEpoch()) {
            this->lineSeries->replace(i, time.toMSecsSinceEpoch(), value);
            this->scatter->replace(i, time.toMSecsSinceEpoch(), value);

        }
    }
}

void MonitoringChart::resetView() {
    QDateTime s;
    QDateTime e;
    s.setMSecsSinceEpoch(xMin - 1000 * 60);
    e.setMSecsSinceEpoch(xMax + 1000 * 60);
    aX->setRange(s, e);
    aY->setRange(yMin - 2, yMax + 2);

}

void MonitoringChart::addX(QDateTimeAxis *axisX) {
    aX = axisX;
    QChart::addAxis(axisX, Qt::AlignBottom);
}

void MonitoringChart::addY(QValueAxis *axisY) {
    aY = axisY;
    QChart::addAxis(axisY, Qt::AlignLeft);
}

MonitoringChart::MonitoringChart(QGraphicsItem *parent, Qt::WindowFlags wFlags) : QChart(parent, wFlags) {
    QLineSeries *ls = new QLineSeries();
    QScatterSeries *scatter = new QScatterSeries();
    scatter->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    scatter->setMarkerSize(15.0);

    this->setAnimationOptions(QChart::SeriesAnimations);
    this->legend()->hide();
    this->addSeries(ls);
    this->addSeries(scatter);


    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTickCount(5);
    axisX->setFormat("dd/MM/yy hh:mm:ss");
    axisX->setTitleText("Date");
    this->addX(axisX);
    lineSeries->attachAxis(axisX);
    scatter->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setLabelFormat("%i");
    axisY->setTitleText("Numero dispositivi rilevati");
    this->addY(axisY);
    lineSeries->attachAxis(axisY);
    scatter->attachAxis(axisY);

}

QLineSeries *MonitoringChart::getLineSeries() {
    return lineSeries;
}

QScatterSeries *MonitoringChart::getScatter() {
    return scatter;
}




