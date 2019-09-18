//
// Created by pieromack on 17/09/19.
//

#ifndef ROOMMONITOR_PLOT1_H
#define ROOMMONITOR_PLOT1_H

#include <QtCharts>


class Plot1 : public QChartView {
    qreal scroll_to_start = 0;
    qreal tick_size = 0;
    qreal one_line_size = 0;
public:
    explicit Plot1(QWidget *parent = nullptr);

    explicit Plot1(QChart *chart, QWidget *parent = nullptr);
    /**
     * Funzione chiamabile dall'esterno utile per definire quante devono essere visualizzate. Serve per impostare
     * i limiti dello scroll.
     * @param i
     */
    void setCategorySize(int i);

protected:
    /**
     * Gestore eventi scroll della rotella del mouse
     * @param event
     */
    void wheelEvent(QWheelEvent *event) override;

    /**
     * Gestore evento Doppio Click del mouse
     * @param event
     */
    void mouseDoubleClickEvent(QMouseEvent *event);

    /**
     * Gestore evento Resize della schermata. Serve per ricalcolare scroll e zoom. Resetta alla posizione iniziale.
     * @param event
     */
    void resizeEvent(QResizeEvent *event) override;

};


#endif //ROOMMONITOR_PLOT1_H
