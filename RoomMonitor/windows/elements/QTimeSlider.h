//
// Created by pieromack on 30/10/19.
//

#ifndef ROOMMONITOR_QTIMESLIDER_H
#define ROOMMONITOR_QTIMESLIDER_H


#include <QtWidgets/QSlider>
#include <windows/common/callout.h>
#include <QtWidgets/QLabel>
#include <windows/classes/PositionDataPlot.h>
#include <windows/classes/LastMac.h>


class QTimeSlider: public QSlider {
    Q_OBJECT
    void mouseMoveEvent(QMouseEvent *ev) override;
    std::vector<PositionDataPlot> data;
    std::vector<QDateTime> keyMap;

public:
    explicit QTimeSlider(QWidget *parent = nullptr);

    void setData(const std::vector<PositionDataPlot> &data);

signals:
    void emitDateMax(PositionDataPlot e);
};


#endif //ROOMMONITOR_QTIMESLIDER_H
