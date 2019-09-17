//
// Created by pieromack on 17/09/19.
//

#ifndef ROOMMONITOR_PLOT1_H
#define ROOMMONITOR_PLOT1_H

#include <QtCharts>


class Plot1 : public QChartView {
public:
    explicit Plot1(QWidget *parent = nullptr);

    explicit Plot1(QChart *chart, QWidget *parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) override;

    void mouseDoubleClickEvent(QMouseEvent *event);
};


#endif //ROOMMONITOR_PLOT1_H
