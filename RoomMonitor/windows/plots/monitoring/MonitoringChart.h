//
// Created by pieromack on 18/09/19.
//

#ifndef ROOMMONITOR_MONITORINGCHART_H
#define ROOMMONITOR_MONITORINGCHART_H

#include <QtCharts>

#include <limits.h>

class MonitoringChart : public QChart {
    QLineSeries *lineSeries;
    QScatterSeries *scatter;
    QDateTimeAxis *aX;
    QValueAxis *aY;
    int yMin = INT_MAX;
    int yMax = -1;
    qreal xMin = 0;
    qreal xMax = 0;



public:
    explicit MonitoringChart(QGraphicsItem *parent = nullptr, Qt::WindowFlags wFlags = Qt::WindowFlags());
    void addSeries(QLineSeries *series);
    void addData(const QDateTime& time, int value);
    void updateData(const QDateTime& time, int value);
    void addX(QDateTimeAxis *axisX);
    void addY(QValueAxis *axisY);
    QLineSeries * getLineSeries();
    QScatterSeries * getScatter();

    void resetView();

    void addSeries(QScatterSeries *series);


};


#endif //ROOMMONITOR_MONITORINGCHART_H
