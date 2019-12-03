//
// Created by pieromack on 01/10/19.
//

#include "AnalysisWorker.h"

void AnalysisWorker::doWork(){
    QString title = "Conteggio presenze lungo periodo";
    QFont f = result->titleFont();
    f.setBold(true);
    result->setTitleFont(f);
    QSettings su{Utility::ORGANIZATION, Utility::APPLICATION};
    int granularity = 60 * 5;
    int bucket = 60;
    int freq = su.value("monitor/min").toInt();
    QDateTime start{};
    start.setSecsSinceEpoch(start_in.toSecsSinceEpoch() / (60 * 5) * (60 * 5));
    QDateTime end{};
    end.setSecsSinceEpoch(end_in.toSecsSinceEpoch() / (60 * 5) * (60 * 5));
    qint64 diff = end.toSecsSinceEpoch() - start.toSecsSinceEpoch();
    freq = su.value("monitor/min").toInt();
    result->setTitle(title);
    bool error = false;
    QSqlDatabase db = Utility::getDB(error);
    if (error) return;
    QSqlQuery q{db};
    /** QUERY_4 **/
    q.prepare(Query::SELECT_TIMING_COUNT_BUCKET_FREQ.arg(su.value("database/table").toString()));
    q.bindValue(":fd", start.toString("yyyy-MM-dd hh:mm:ss"));
    q.bindValue(":sd", end.toString("yyyy-MM-dd hh:mm:ss"));
    q.bindValue(":sec", granularity);
    q.bindValue(":bucket", bucket);
    q.bindValue(":freq", freq);
    q.bindValue(":lar", su.value("room/width").toString());
    q.bindValue(":lun", su.value("room/height").toString());

    if (!q.exec()) {
        qCritical() << q.lastError();
        db.close();
        emit warning(Strings::ERR_DB, Strings::ERR_DB_MSG, q.lastError().text());
        emit finished();
        return;
    }

    QDateTime temp = start;
    qint64 current = 0;
    qint64 last = 0;
    bool before_data = false;
    QMap<QDateTime, int> timeline;
    for (int j = 0; temp.addSecs(granularity * j) < end; j++) {
        timeline.insert(temp.addSecs(granularity * j), 0);
    }
    while (q.next()) {
        QDateTime timing = q.value(0).toDateTime();
        int count_mac = q.value(1).toInt();
        timeline.insert(timing, count_mac);
    }

    QMapIterator<QDateTime, int> itt(timeline);
    int total = timeline.size();
    int curr = 0;
    while (itt.hasNext()) {
        curr++;
        itt.next();
        result->addData(itt.key(), itt.value());
        current = curr / total * 100;
        if (last + 10 <= current) {
            emit updateProgress(current);
            last = current;
        }
    }
    emit resultReady(result);

    q.clear();
    /** QUERY_5B **/
    q.prepare(Query::SELECT_MAC_COUNT_HIDDEN_ORDERBY_FREQ.arg(su.value("database/table").toString()));
    q.bindValue(":fd", start.toString("yyyy-MM-dd hh:mm:ss"));
    q.bindValue(":sd", end.toString("yyyy-MM-dd hh:mm:ss"));
    q.bindValue(":bucket", bucket);
    q.bindValue(":sec", granularity);
    q.bindValue(":freq", freq);
    q.bindValue(":lar", su.value("room/width").toString());
    q.bindValue(":lun", su.value("room/height").toString());


    if (!q.exec()) {
        qCritical() << q.lastError();
        db.close();
        emit warning(Strings::ERR_DB, Strings::ERR_DB_MSG, q.lastError().text());
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
    q.prepare(Query::SELECT_MAC_COUNT_HIDDEN_ORDERBY_MAC.arg(su.value("database/table").toString()));
    q.bindValue(":fd", start.toString("yyyy-MM-dd hh:mm:ss"));
    q.bindValue(":sd", end.toString("yyyy-MM-dd hh:mm:ss"));
    q.bindValue(":bucket", bucket);
    q.bindValue(":sec", granularity);
    q.bindValue(":freq", freq);
    q.bindValue(":lar", su.value("room/width").toString());
    q.bindValue(":lun", su.value("room/height").toString());

    if (!q.exec()) {
        qCritical() << q.lastError();
        db.close();
        emit warning(Strings::ERR_DB, Strings::ERR_DB_MSG, q.lastError().text());
        emit finished();
        return;
    }

    while (q.next()) {
        QString maco = q.value(0).toString();
        int frequency = q.value(1).toInt();
        bool hidden = q.value(2).toBool();
        emit addMac(maco, frequency, hidden);
    }
    q.clear();

    /** QUERY_1**/
    /* Con questa query otteniamo una entry per ogni mac e per ogni granularità con la posizione mediata in base a
     * tutti i pacchetti trovati nel range selezionato */
    q.prepare(Query::SELECT_MAC_TIMING_AVGPOS.arg(su.value("database/table").toString()));
    q.bindValue(":fd", start.toString("yyyy-MM-dd hh:mm:ss"));
    q.bindValue(":sd", end.toString("yyyy-MM-dd hh:mm:ss"));
    q.bindValue(":sec", granularity);
    q.bindValue(":lar", su.value("room/width").toString());
    q.bindValue(":lun", su.value("room/height").toString());
    if (!q.exec()) {
        qCritical() << q.lastError();
        db.close();
        emit finished();
        return;
    }

    // Creo una mappa da Data a vettore di Mac e relative posizioni
    std::map<QDateTime, std::vector<LastMac>> map;
    QDateTime first;
    while (q.next()) {
        QString maco = q.value(0).toString();
        QDateTime date = q.value(1).toDateTime();
        qreal posx = q.value(2).toReal();
        qreal posy = q.value(3).toReal();
        auto el = map.find(date);
        if (el != map.end()) {
            LastMac p{maco, date, posx, posy};
            el->second.push_back(p);
        } else {
            std::vector<LastMac> v;
            LastMac p{maco, date, posx, posy};
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
