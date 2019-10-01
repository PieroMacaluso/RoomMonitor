//
// Created by pieromack on 01/10/19.
//

#ifndef ROOMMONITOR_ANALYSISWORKER_H
#define ROOMMONITOR_ANALYSISWORKER_H


#include <QObject>
#include <windows/plots/monitoring/MonitoringChart.h>
#include <QtSql>
#include <utility>

class AnalysisWorker : public QObject {
Q_OBJECT
    QDateTime start_in;
    QDateTime end_in;
    MonitoringChart *result;
public:
    AnalysisWorker(QDateTime start_in, QDateTime end_in, MonitoringChart *result) : start_in(std::move(start_in)),
                                                                                    end_in(end_in), result(result) {}

    static QSqlDatabase getDB() {
        QSettings su{"VALP", "RoomMonitoring"};
        auto name = "thread_db";
        if (QSqlDatabase::contains(name)) {
            qDebug() << "Not Contain";
            return QSqlDatabase::database(name);
        } else {
            qDebug() << "Contain";
            QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", name);
            db.setHostName(su.value("database/host").toString());
            db.setDatabaseName(su.value("database/name").toString());
            db.setPort(su.value("database/port").toInt());
            db.setUserName(su.value("database/user").toString());
            db.setPassword(su.value("database/pass").toString());
            if (!db.open())
                qDebug() << db.lastError();
            return db;
        }
    }

public slots:

    void doWork() {
        QString title = "Analisi di lungo periodo - ";
        QFont f = result->titleFont();
        f.setBold(true);
        result->setTitleFont(f);
        QSettings su{"VALP", "RoomMonitoring"};
        int granularity = 60*5;
        int bucket = 1;
        int freq = 3;
        QDateTime start{};
        start.setSecsSinceEpoch(start_in.toSecsSinceEpoch() / (60 * 5) * (60 * 5));
        QDateTime end{};
        end.setSecsSinceEpoch(end_in.toSecsSinceEpoch() / (60 * 5) * (60 * 5));
        qint64 diff = end.toSecsSinceEpoch() - start.toSecsSinceEpoch();
        if (diff >= 10*12*30*24*60*60){
            result->setTitle(title + "Modalità: DECENNIO - Granularità: 30d|1d");
            granularity = 60*60*24*30;
            bucket = 60*60*24;
            freq = 1;
        }
        else if (diff >= 12*30*24*60*60){
            result->setTitle(title + "Modalità: ANNO - Granularità: 1w|1d");
            granularity = 60*60*24*7;
            bucket = 60*60*24;
            freq = 1;

        } else if (diff > 31*24*60*60){
            result->setTitle(title + "Modalità: MESE - Granularità: 1d|1h");
            granularity = 60*60*24;
            bucket = 60*60;
            freq = 1;

        } else {
            result->setTitle(title + "Modalità: GIORNO - Granularità: 5m|1m");
            granularity = 60*5;
            bucket = 60;
            freq = 3;
        }
        QSqlDatabase db = getDB();
        QSqlQuery q{db};
        q.prepare(
                "SELECT timing, COUNT(*)\n"
                "FROM (SELECT mac_addr,\n"
                "             FROM_UNIXTIME(UNIX_TIMESTAMP(timing) - MOD(UNIX_TIMESTAMP(timing), :sec)) AS timing,\n"
                "             COALESCE(COUNT(DISTINCT timing), 0)                                                   AS freq\n"
                "      FROM (SELECT mac_addr,\n"
                "                   FROM_UNIXTIME(UNIX_TIMESTAMP(timestamp) - MOD(UNIX_TIMESTAMP(timestamp), :bucket)) AS timing,\n"
                "                   avg(pos_x)                                                                    AS pos_x,\n"
                "                   avg(pos_y)                                                                    AS pos_y\n"
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
        if (!q.exec())
            qDebug() << q.lastError();

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
        db.close();
        emit resultReady(result);
    }

signals:

    void resultReady(MonitoringChart *chart);

    void updateProgress(qint64 perc);

};


#endif //ROOMMONITOR_ANALYSISWORKER_H
