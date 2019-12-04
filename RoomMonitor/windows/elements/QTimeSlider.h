#ifndef ROOMMONITOR_QTIMESLIDER_H
#define ROOMMONITOR_QTIMESLIDER_H


#include <QtWidgets/QSlider>
#include <windows/common/callout.h>
#include <QtWidgets/QLabel>
#include <windows/classes/PositionDataPlot.h>
#include <windows/classes/LastMac.h>


class QTimeSlider: public QSlider {
    Q_OBJECT
    /**
     * Mouse Event
     * @param ev Event
     */
    void mouseMoveEvent(QMouseEvent *ev) override;
    /**
     * Vettore si posizioni
     */
    std::vector<PositionDataPlot> data;

public:
    /**
     * Default constructor
     * @param parent
     */
    explicit QTimeSlider(QWidget *parent = nullptr);

    /**
     * Set Data in the QTimeSlider
     * @param data
     */
    void setData(const std::vector<PositionDataPlot> &data);

signals:
    /**
     * Emit the maximum date for the outside
     * @param e
     */
    void emitDateMax(PositionDataPlot e);
};


#endif //ROOMMONITOR_QTIMESLIDER_H
