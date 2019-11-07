//
// Created by pieromack on 09/10/19.
//

#ifndef ROOMMONITOR_LIVECHART_H
#define ROOMMONITOR_LIVECHART_H

#include <QtCharts>
#include <monitoring/Board.h>
#include <windows/classes/LastMac.h>

class LiveChart : public QChart {
    std::vector<LastMac> devices_v{};
    std::vector<Board> boards_v{};
    QScatterSeries *boards;
    QScatterSeries *devices;
    QValueAxis *aX;
    QValueAxis *aY;
    qreal xMax = 0;
    qreal yMax = 0;
    int currentPos = 0;

public:
    explicit LiveChart(QGraphicsItem *parent = nullptr, Qt::WindowFlags wFlags = Qt::WindowFlags());
    void addBoardsSeries(QScatterSeries *series);
    void addDevicesSeries(QScatterSeries *series);
    void addBoard(qreal xValue, qreal yValue);
    void addDevice(qreal xValue, qreal yValue);
    void addX(QValueAxis *axisX);
    void addY(QValueAxis *axisY);
    QScatterSeries * getBoardsScatter();
    QScatterSeries * getDevicesScatter();

    void resetView();

    qreal getXMax() const;

    qreal getYMax() const;

    void fillBoards(std::vector<Board> newData);
    void fillDevices(QMap<QString, LastMac> &newData);
    void fillDevicesV(std::vector<LastMac> &newData);



    const std::vector<LastMac> & getDevices() const;
    const std::vector<Board> &getBoards() const;

};


#endif //ROOMMONITOR_LIVECHART_H
