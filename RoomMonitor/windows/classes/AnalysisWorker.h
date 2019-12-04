#ifndef ROOMMONITOR_ANALYSISWORKER_H
#define ROOMMONITOR_ANALYSISWORKER_H


#include <QObject>
#include <windows/plots/monitoring/MonitoringChart.h>
#include <QtSql>
#include <utility>
#include <windows/plots/mac/MacChart.h>
#include <windows/plots/mac/MacChart.h>
#include <Utility.h>
#include <windows/plots/live/LiveChart.h>
#include <windows/elements/QTimeSlider.h>
#include <windows/elements/QMapSlider.h>
#include "PositionDataPlot.h"

class AnalysisWorker : public QObject {
Q_OBJECT

    /**
     * Start date
     */
    QDateTime start_in;
    /**
     * End date
     */
    QDateTime end_in;

    /**
     * Pointer to MonitoringChart
     */
    MonitoringChart *result;

    /**
     * MacChart pointer
     */
    MacChart *macPlot;

    /**
     * Main Time Slider pointer
     */
    QMapSlider *timeSlider;

public:
    /**
     * Costruttore del thread
     * @param start_in
     * @param end_in
     * @param result
     * @param macPlot
     * @param timeSlider
     */
    AnalysisWorker(QDateTime start_in, QDateTime end_in, MonitoringChart *result, MacChart *macPlot,
                   QMapSlider *timeSlider)
            : start_in(std::move(start_in)),
              end_in(end_in), result(result), macPlot(macPlot), timeSlider(timeSlider) {}

public slots:

    /**
     * This is the main function of the thread. It will query the database to scrape information and data about the range
     * of days requested.
     */
    void doWork();

signals:

    /**
     * Signalling the readiness of results. It brings a pointer to the MonitoringChart
     * @param chart
     */
    void resultReady(MonitoringChart *chart);

    /**
     * It signals readiness of MacPlot
     * @param mac
     * @param frequency
     * @param macPlot
     */
    void macPlotReady(QStringList mac, QStringList frequency, MacChart *macPlot);

    /**
     * Signal end of the process
     */
    void finished();

    /**
     * Emit warnings
     * @param title
     * @param text
     * @param error
     */
    void warning(QString title, QString text, QString error);

    /**
     * Updating the progress to update progress bar in the graphic thread
     * @param perc
     */
    void updateProgress(qint64 perc);

    /**
     * Signal Initialize MAC table
     */
    void initializeMacSituation();

    /**
     * Signal the add of a MAC to the table
     * @param mac
     * @param frequency
     * @param hidden
     */
    void addMac(QString mac, int frequency, bool hidden);
};


#endif //ROOMMONITOR_ANALYSISWORKER_H
