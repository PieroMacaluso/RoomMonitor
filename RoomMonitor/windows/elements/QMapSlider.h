#ifndef ROOMMONITOR_QMAPSLIDER_H
#define ROOMMONITOR_QMAPSLIDER_H

#include <QtWidgets/QSlider>
#include <windows/common/callout.h>
#include <QtWidgets/QLabel>
#include <windows/classes/PositionDataPlot.h>
#include <windows/classes/LastMac.h>

class QMapSlider : public QSlider {
Q_OBJECT

    /**
     * Action on mouse event
     * @param ev    Event
     */
    void mouseMoveEvent(QMouseEvent *ev) override;

    /**
     * Vector of QDateTime and last MAC vector
     */
    std::map<QDateTime, std::vector<LastMac>> map;

    /**
     * Vector di Date per le chiavi
     */
    std::vector<QDateTime> keyMap;

public:
    /**
     * Costruttore predefinito
     * @param parent
     */
    explicit QMapSlider(QWidget *parent = nullptr);

    /**
     * Check if the map is empty
     * @return  bool    true if it is empty, false otherwise
     */
    bool isMapEmpty();

    /**
     * Function to setup the Map
     * @param mapOut    map of QDateTime and vector of LastMac
     */
    void setMap(const std::map<QDateTime, std::vector<LastMac>> &mapOut);

    /**
     * Get Map Index
     * @param value     index of the map
     * @return      vector of the index
     */
    std::vector<LastMac> getMapIndex(int value);

    /**
     * Get Date Key
     * @param value     index of the map
     * @return      QDateTime
     */
    QDateTime getKeyIndex(int value);

signals:

    /**
     * Signal to emit the Maximum date for the Last MAC
     * @param e
     */
    void emitDateMax(std::vector<LastMac> e);

    /**
     * Signal of initialization
     */
    void initialized();
};


#endif //ROOMMONITOR_QMAPSLIDER_H
