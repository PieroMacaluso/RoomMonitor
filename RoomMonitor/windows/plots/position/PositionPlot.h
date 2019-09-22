//
// Created by pieromack on 22/09/19.
//

#ifndef ROOMMONITOR_POSITIONPLOT_H
#define ROOMMONITOR_POSITIONPLOT_H

#include <QtCharts>
#include <windows/classes/PositionDataPlot.h>

class PositionPlot: public QChart  {
    std::vector<PositionDataPlot> data;
    QLineSeries *lineSeries;
    QScatterSeries *scatter;
    QValueAxis *aX;
    QValueAxis *aY;
    qreal xMax = 0;
    qreal yMax = 0;
    int currentPos = 0;
public:
    explicit PositionPlot(QGraphicsItem *parent = nullptr, Qt::WindowFlags wFlags = Qt::WindowFlags());
    void addSeries(QLineSeries *series);
    void addData(qreal xValue, qreal yValue);
    void addX(QValueAxis *axisX);
    void addY(QValueAxis *axisY);
    QLineSeries * getLineSeries();
    QScatterSeries * getScatter();

    void resetView();

    void addSeries(QScatterSeries *series);

    qreal getXMax() const;

    qreal getYMax() const;

    void fillData(std::vector<PositionDataPlot> newData);

    void sliderChanged(int position);

    const PositionDataPlot &getData(int index) const;

    const std::vector<PositionDataPlot> &getData1() const;
};


#endif //ROOMMONITOR_POSITIONPLOT_H
