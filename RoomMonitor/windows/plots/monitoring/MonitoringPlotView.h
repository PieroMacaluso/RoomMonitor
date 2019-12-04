#ifndef ROOMMONITOR_MONITORINGPLOTVIEW_H
#define ROOMMONITOR_MONITORINGPLOTVIEW_H

#include <QtCharts>
#include "MonitoringChart.h"
#include "windows/common/callout.h"


class MonitoringPlotView : public QChartView {
    /**
     * Grafico contenuto dalla View
     */
    MonitoringChart * chart;

    /**
     * Callout utilizzato da MouseHover
     */
    Callout *m_tooltip = nullptr;

public:
    /**
     * Costruttori che ricevono il widget parente per la liberazione successiva
     * @param parent
     */
    explicit MonitoringPlotView(QWidget *parent = nullptr);

    explicit MonitoringPlotView(QChart *chart, QWidget *parent = nullptr);

    /**
     * Settaggio del puntatore al grafico da visualizzare
     * @param chart grafico MonitoringChart
     */
    void setChart(MonitoringChart *chart);

    /**
     * Gestione evento doppio click
     * @param event
     */
    void mouseDoubleClickEvent(QMouseEvent *event);

    /**
     * Gestione rotella del mouse
     * @param event
     */
    void wheelEvent(QWheelEvent *event);
    /**
     * Gestione callout
     * @param event
     */
    void tooltip(QPointF point, bool state);

    MonitoringChart *getChart() const;
};


#endif //ROOMMONITOR_MONITORINGPLOTVIEW_H
