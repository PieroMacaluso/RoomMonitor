//
// Created by pieromack on 04/11/19.
//

#ifndef ROOMMONITOR_QMAPSLIDER_H
#define ROOMMONITOR_QMAPSLIDER_H

#include <QtWidgets/QSlider>
#include <windows/common/callout.h>
#include <QtWidgets/QLabel>
#include <windows/classes/PositionDataPlot.h>
#include <windows/classes/LastMac.h>

class QMapSlider : public QSlider {
Q_OBJECT

    void mouseMoveEvent(QMouseEvent *ev) override;

    std::map<QDateTime, std::vector<LastMac>> map;
    std::vector<QDateTime> keyMap;

public:
    explicit QMapSlider(QWidget *parent = nullptr);

    bool isMapEmpty();

    void setMap(const std::map<QDateTime, std::vector<LastMac>> &mapOut);

    std::vector<LastMac> getMapIndex(int value);

    QDateTime getKeyIndex(int value);

signals:

    void emitDateMax(std::vector<LastMac> e);

    void initialized();
};


#endif //ROOMMONITOR_QMAPSLIDER_H
