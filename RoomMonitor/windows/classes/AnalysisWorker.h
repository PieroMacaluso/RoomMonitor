//
// Created by pieromack on 01/10/19.
//

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
#include "MacLastPos.h"


class AnalysisWorker : public QObject {
Q_OBJECT
    QDateTime start_in;
    QDateTime end_in;
    MonitoringChart *result;
    MacChart *macPlot;
    QMapSlider *timeSlider;

public:
    AnalysisWorker(QDateTime start_in, QDateTime end_in, MonitoringChart *result, MacChart *macPlot,
                   QMapSlider *timeSlider)
            : start_in(std::move(start_in)),
              end_in(end_in), result(result), macPlot(macPlot), timeSlider(timeSlider) {}

public slots:
    void doWork() {
        QString title = "Analisi di lungo periodo - ";
        QFont f = result->titleFont();
        f.setBold(true);
        result->setTitleFont(f);
        QSettings su{"VALP", "RoomMonitoring"};
        int granularity = 60 * 5;
        int bucket = 1;
        int freq = su.value("monitor/min").toInt();
        QDateTime start{};
        start.setSecsSinceEpoch(start_in.toSecsSinceEpoch() / (60 * 5) * (60 * 5));
        QDateTime end{};
        end.setSecsSinceEpoch(end_in.toSecsSinceEpoch() / (60 * 5) * (60 * 5));
        qint64 diff = end.toSecsSinceEpoch() - start.toSecsSinceEpoch();
        // TODO: Frequenza minima personalizzata per ogni granularità? Issue #23
        if (diff >= 10 * 12 * 30 * 24 * 60 * 60) {
            result->setTitle(title + "Modalità: DECENNIO - Granularità: 30d|1d");
            granularity = 60 * 60 * 24 * 30;
            bucket = 60 * 60 * 24;
            freq = 1;
        } else if (diff >= 12 * 30 * 24 * 60 * 60) {
            result->setTitle(title + "Modalità: ANNO - Granularità: 1w|1d");
            granularity = 60 * 60 * 24 * 7;
            bucket = 60 * 60 * 24;
            freq = 1;

        } else if (diff > 31 * 24 * 60 * 60) {
            result->setTitle(title + "Modalità: MESE - Granularità: 1d|1h");
            granularity = 60 * 60 * 24;
            bucket = 60 * 60;
            freq = 1;

        } else {
            freq = su.value("monitor/min").toInt();
            result->setTitle(
                    title + "Modalità: GIORNO - Granularità: 5m|" + QString::fromStdString(std::to_string(freq)) + "m");
            granularity = 60 * 5;
            bucket = 60;
        }
        bool error = false;
        QSqlDatabase db = Utility::getDB(error);
        if (error) return;
        QSqlQuery q{db};
        /** QUERY_4 **/
        q.prepare(
                "SELECT timing, COUNT(*)\n"
                "FROM (SELECT mac_addr,\n"
                "             FROM_UNIXTIME(UNIX_TIMESTAMP(timing) - MOD(UNIX_TIMESTAMP(timing), :sec)) AS timing,\n"
                "             COUNT(DISTINCT timing)                                                  AS freq\n"
                "      FROM (SELECT mac_addr,\n"
                "                   FROM_UNIXTIME(UNIX_TIMESTAMP(timestamp) - MOD(UNIX_TIMESTAMP(timestamp), :bucket)) AS timing\n"
                "            FROM " + su.value("database/table").toString() + "\n"
                                                                              "            WHERE timestamp BETWEEN :fd AND :sd\n"
                                                                              "            GROUP BY mac_addr, UNIX_TIMESTAMP(timestamp) DIV :bucket\n"
                                                                              "            ORDER BY timing) as eL\n"
                                                                              "      GROUP BY mac_addr, FROM_UNIXTIME(UNIX_TIMESTAMP(timing) - MOD(UNIX_TIMESTAMP(timing), :sec))\n"
                                                                              "     ) AS mac_count\n"
                                                                              "WHERE mac_count.freq >= :freq\n"
                                                                              "GROUP BY timing\n"
                                                                              "ORDER BY timing;");
        q.bindValue(":fd", start.toString("yyyy-MM-dd hh:mm:ss"));
        q.bindValue(":sd", end.toString("yyyy-MM-dd hh:mm:ss"));
        q.bindValue(":sec", granularity);
        q.bindValue(":bucket", bucket);
        q.bindValue(":freq", freq);
        if (!q.exec()) {
            qDebug() << q.lastError();
            db.close();
            emit warning(Strings::ERR_DB,
                         "Si è verificato un errore nel Database, ti invitiamo a modificare le Impostazioni e riprovare", q.lastError().text());
            emit finished();
            return;
        }


        QDateTime temp = start;
        int current = 0;
        int last = 0;

        while (q.next()) {
            QDateTime timing = q.value(0).toDateTime();
            if (!temp.isNull()) {
                int j = 1;
                while (temp.addSecs(granularity * j) < timing) {
                    result->addData(temp.addSecs(granularity * j), 0);
                    j++;
                    //qDebug() << temp.addSecs(60 * 5 * j);
                    current = (temp.addSecs(granularity * j).toSecsSinceEpoch() - start.toSecsSinceEpoch() * 1.0) /
                              (end.toSecsSinceEpoch() - start.toSecsSinceEpoch() * 1.0) * 100;
                    if (last + 10 <= current) {
                        emit updateProgress(current);
                        last = current;
                    }

                }
            }
            int count_mac = q.value(1).toInt();
            result->addData(timing, count_mac);
            temp = timing;
            current = (timing.toSecsSinceEpoch() - start.toSecsSinceEpoch() * 1.0) /
                      (end.toSecsSinceEpoch() - start.toSecsSinceEpoch() * 1.0) * 100;
            if (last + 10 <= current) {
                emit updateProgress(current);
                last = current;
            }
        }
        int j = 1;
        while (temp.addSecs(granularity * j) <= end) {
            result->addData(temp.addSecs(granularity * j), 0);
            j++;
            current = (temp.addSecs(granularity * j).toSecsSinceEpoch() - start.toSecsSinceEpoch() * 1.0) /
                      (end.toSecsSinceEpoch() - start.toSecsSinceEpoch() * 1.0) * 100;
            if (last + 10 <= current) {
                emit updateProgress(current);
                last = current;
            }
        }
        emit resultReady(result);

        q.clear();
        /** QUERY_5B **/
        q.prepare("SELECT mac_addr,\n"
                  "       COUNT(*) AS freq,\n"
                  "       hidden\n"
                  "FROM (SELECT mac_addr,\n"
                  "             FROM_UNIXTIME(UNIX_TIMESTAMP(timing) - MOD(UNIX_TIMESTAMP(timing), :sec)) AS timing,\n"
                  "             COUNT(DISTINCT timing)                                              AS freq,\n"
                  "             hidden\n"
                  "      FROM (SELECT mac_addr,\n"
                  "                   FROM_UNIXTIME(UNIX_TIMESTAMP(timestamp) - MOD(UNIX_TIMESTAMP(timestamp), :freq)) AS timing,\n"
                  "                   COUNT(DISTINCT timestamp),\n"
                  "                   hidden\n"
                  "            FROM " + su.value("database/table").toString() + "\n"
                                                                                "            WHERE timestamp BETWEEN :fd AND :sd\n"
                                                                                "            GROUP BY mac_addr, UNIX_TIMESTAMP(timestamp) DIV :bucket\n"
                                                                                "            ORDER BY timing) as eL\n"
                                                                                "      GROUP BY mac_addr, FROM_UNIXTIME(UNIX_TIMESTAMP(timing) - MOD(UNIX_TIMESTAMP(timing), :sec))\n"
                                                                                "     ) AS mac_count\n"
                                                                                "WHERE mac_count.freq >= :freq\n"
                                                                                "GROUP BY mac_addr\n"
                                                                                "ORDER BY freq DESC;");
        q.bindValue(":fd", start.toString("yyyy-MM-dd hh:mm:ss"));
        q.bindValue(":sd", end.toString("yyyy-MM-dd hh:mm:ss"));
        q.bindValue(":bucket", bucket);
        q.bindValue(":sec", granularity);
        q.bindValue(":freq", freq);


        if (!q.exec()) {
            qDebug() << q.lastError();
            db.close();
            emit finished();
            return;
        }

        QStringList mac{};
        QStringList frequency_mac{};
        QVector<MacOccurrence> macs;
        int i = 0;
        while (q.next()) {
            mac << q.value(0).toString();
            frequency_mac << q.value(1).toString();
            i++;
        }
        if (i) {
            emit macPlotReady(mac, frequency_mac, macPlot);
        }
        q.clear();
        emit initializeMacSituation();
        /** QUERY_6 **/
        q.prepare("SELECT mac_addr,\n"
                  "       COUNT(*) AS freq,\n"
                  "       hidden\n"
                  "FROM (SELECT mac_addr,\n"
                  "             FROM_UNIXTIME(UNIX_TIMESTAMP(timing) - MOD(UNIX_TIMESTAMP(timing), :sec)) AS timing,\n"
                  "             COUNT(DISTINCT timing)                                              AS freq,\n"
                  "             hidden\n"
                  "      FROM (SELECT mac_addr,\n"
                  "                   FROM_UNIXTIME(UNIX_TIMESTAMP(timestamp) - MOD(UNIX_TIMESTAMP(timestamp), :bucket)) AS timing,\n"
                  "                   COALESCE(COUNT(DISTINCT timestamp)  ,0),\n"
                  "                   hidden\n"
                  "            FROM " + su.value("database/table").toString() + "\n"
                                                                                "            WHERE timestamp BETWEEN :fd AND :sd\n"
                                                                                "            GROUP BY mac_addr, UNIX_TIMESTAMP(timestamp) DIV :bucket\n"
                                                                                "            ORDER BY timing) as eL\n"
                                                                                "      GROUP BY mac_addr, FROM_UNIXTIME(UNIX_TIMESTAMP(timing) - MOD(UNIX_TIMESTAMP(timing), :sec))\n"
                                                                                "     ) AS mac_count\n"
                                                                                "WHERE mac_count.freq >= :freq\n"
                                                                                "GROUP BY mac_addr\n"
                                                                                "ORDER BY mac_addr;");
        q.bindValue(":fd", start.toString("yyyy-MM-dd hh:mm:ss"));
        q.bindValue(":sd", end.toString("yyyy-MM-dd hh:mm:ss"));
        q.bindValue(":bucket", bucket);
        q.bindValue(":sec", granularity);
        q.bindValue(":freq", freq);
        if (!q.exec()) {
            qDebug() << q.lastError();
            db.close();
            emit finished();
            return;
        }

        while (q.next()) {
            QString mac = q.value(0).toString();
            int frequency = q.value(1).toInt();
            bool hidden = q.value(2).toBool();
            emit addMac(mac, frequency, hidden);
        }
        q.clear();

        /** QUERY_1**/
        /* Con questa query otteniamo una entry per ogni mac e per ogni granularità con la posizione mediata in base a
         * tutti i pacchetti trovati nel range selezionato */
        q.prepare("SELECT mac_addr,\n"
                  "       FROM_UNIXTIME(UNIX_TIMESTAMP(timestamp) - MOD(UNIX_TIMESTAMP(timestamp), :sec)) AS timing,\n"
                  "       avg(pos_x)                                                                    as pos_x,\n"
                  "       avg(pos_y)                                                                    as pos_y\n"
                  "FROM " + su.value("database/table").toString() + "\n"
                                                                    "WHERE timestamp > :fd\n"
                                                                    "  AND timestamp < :sd\n"
                                                                    "GROUP BY mac_addr, UNIX_TIMESTAMP(timestamp) DIV :sec\n"
                                                                    "ORDER BY timing;");
        q.bindValue(":fd", start.toString("yyyy-MM-dd hh:mm:ss"));
        q.bindValue(":sd", end.toString("yyyy-MM-dd hh:mm:ss"));
        q.bindValue(":sec", granularity);
        if (!q.exec()) {
            qDebug() << q.lastError();
            db.close();
            emit finished();
            return;
        }

        // Creo una mappa da Data a vettore di Mac e relative posizioni
        std::map<QDateTime, std::vector<LastMac>> map;
        while (q.next()) {
            QString mac = q.value(0).toString();
            QDateTime date = q.value(1).toDateTime();
            qreal posx = q.value(2).toReal();
            qreal posy = q.value(3).toReal();
            auto el = map.find(date);
            if (el != map.end()) {
                LastMac p{mac, date, posx, posy};
                el->second.push_back(p);
            } else {
                std::vector<LastMac> v;
                LastMac p{mac, date, posx, posy};
                v.push_back(p);
                map.insert(std::make_pair(date, v));
            }
        }
        // Setto la mappa all'interno dello slider
        timeSlider->setMap(map);
        timeSlider->setSliderPosition(0);

        db.close();
        emit finished();

    }

signals:

    void resultReady(MonitoringChart *chart);

    void macPlotReady(QStringList mac, QStringList frequency, MacChart *macPlot);

    void finished();

    void warning(QString title, QString text, QString error);

    void updateProgress(qint64 perc);

    void initializeMacSituation();

    void addMac(QString mac, int frequency, bool hidden);


};


#endif //ROOMMONITOR_ANALYSISWORKER_H
