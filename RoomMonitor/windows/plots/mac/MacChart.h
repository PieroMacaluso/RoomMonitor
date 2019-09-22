//
// Created by pieromack on 17/09/19.
//

#ifndef ROOMMONITOR_MACCHART_H
#define ROOMMONITOR_MACCHART_H

#include <QtCharts>
#include <map>
#include <algorithm>
#include <windows/classes/MacOccurrence.h>



class MacChart : public QChart {
    QHorizontalBarSeries * barSeries{};
    QBarCategoryAxis *aY;
    QValueAxis *aX;
    QStringList listMac;

public:
    explicit MacChart(QGraphicsItem *parent = nullptr, Qt::WindowFlags wFlags = Qt::WindowFlags());
    QHorizontalBarSeries *getBarSeries();
    QStringList getMacs();
    void fillChart(QVector<MacOccurrence> macs);
    void resetView(int maxMac);


    void addSeries(QHorizontalBarSeries *series);

    qreal xMin;
    qreal xMax;

};


#endif //ROOMMONITOR_MACCHART_H
