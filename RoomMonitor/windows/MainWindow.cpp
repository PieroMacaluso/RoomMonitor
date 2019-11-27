//
// Created by pieromack on 11/09/19.
//

#include <windows/plots/mac/MacChart.h>
#include <windows/classes/MacOccurrence.h>
#include <windows/classes/PositionDataPlot.h>
#include <windows/classes/AnalysisWorker.h>
#include <Styles.h>
#include "MainWindow.h"
#include "SettingDialog.h"

MainWindow::MainWindow() {
    ui.setupUi(this);
    setupConnect();
    QSettings su{Utility::ORGANIZATION, Utility::APPLICATION};
    ui.actionMonitoring->triggered(true);
}

void MainWindow::setupConnect() {
    // Disabilita pulsante di STOP
    ui.stopButton->setDisabled(true);

    // Imposta range di 5 minuti
    ui.startDate->setDateTime(QDateTime::currentDateTime());
    ui.endDate->setDateTime(QDateTime::currentDateTime().addSecs(60 * 5));

    // Connetti timeout liveGraph a addLiveData
    connect(&s, &MonitoringServer::aggregated, this, &MainWindow::addLiveData);

    // Connect Aggregated per aggiornare grafico live e MAC
    connect(&s, &MonitoringServer::aggregated, this, &MainWindow::genLiveData);

    // Arrotonda di 5 minuti i valori inseriti nella QDateTimeEdit
    connect(ui.startDate, &QDateTimeEdit::dateTimeChanged, [&](QDateTime dateTime) {
        qint64 ts = dateTime.toSecsSinceEpoch() / (60 * 5) * (60 * 5);
        QDateTime t;
        t.setSecsSinceEpoch(ts);
        ui.startDate->setDateTime(t);
    });

    // Arrotonda di 5 minuti i valori inseriti nella QDateTimeEdit
    connect(ui.endDate, &QDateTimeEdit::dateTimeChanged, [&](QDateTime dateTime) {
        qint64 ts = dateTime.toSecsSinceEpoch() / (60 * 5) * (60 * 5);
        QDateTime t;
        t.setSecsSinceEpoch(ts);
        ui.endDate->setDateTime(t);
    });

    // Click Start Button
    QObject::connect(ui.startButton, &QPushButton::clicked, [&]() {
        try {
            if (Utility::getBoards().size() > 1) {
                bool error = false;
                QSqlDatabase db = Utility::getDB(error);
                if (error) return;
                if (!Utility::testTable(db)) return;
                if (!s.start()) return;
                ui.startButton->setDisabled(true);
                ui.stopButton->setDisabled(false);
                // Pulizia tabella Ultimi MAC
                lastMacs.clear();
                i_time = 0;
                setupMonitoringPlot();
                setupLivePlot();
            }
        } catch (std::exception &e) {
            // Does not started signal
            Utility::warningMessage(Strings::SRV_NOT_STARTED, Strings::SRV_NOT_STARTED, Strings::SRV_NOT_STARTED);
            return;
        }
    });

    // Click Search Button
    QObject::connect(ui.searchButton, &QPushButton::clicked, [&]() {
        if (ui.startDate->dateTime().addSecs(60 * 5) > ui.endDate->dateTime() ||
            ui.startDate->dateTime().addDays(31) < ui.endDate->dateTime()) {
            QMessageBox m{};
            m.setStandardButtons(QMessageBox::Close);
            m.setWindowTitle(Strings::RANGE_ERROR);
            m.setIcon(QMessageBox::Warning);
            m.setText(Strings::RANGE_ERROR_MSG);
            m.exec();
            return;
        }
        dataAnalysis();
    });

    // Click Analysis Button
    QObject::connect(ui.actionAnalysis, &QAction::triggered, [&]() {
        if (s.isRunning() && Utility::yesNoMessage(this, Strings::ANA_RUNNING, Strings::ANA_RUNNING_MSG)) {
            s.stop();
            ui.startButton->setDisabled(false);
            ui.stopButton->setDisabled(true);
        }

        // Chiusura monitoraggio
        // Chiudi Dock Monitoraggio
        ui.monitoringManagerDock->setVisible(false);
        ui.lastMacDock->setVisible(false);
        ui.actionMonitoringDock->setEnabled(false);
        ui.actionlastMacDock->setEnabled(false);
        // Disabilita azione Analisi
        ui.actionAnalysis->setVisible(false);

        // Apertura Analisi
        // Apri Dock Analisi
        ui.macDetectedDock->setVisible(true);
        ui.rangeSelectorDock->setVisible(true);
        ui.actionMacSituationDock->setEnabled(true);
        ui.actionRangeDock->setEnabled(true);

        ui.actionMonitoring->setVisible(true);

        // Cambia plot
        ui.monitoringWidget->setVisible(false);
        ui.analysisWidget->setVisible(true);

        ui.title->setText(Styles::HEADER.arg(Strings::ANA));
        setupAnalysisPlot();
        initializeMacSituationList();
    });

    // Click Monitoring Button
    QObject::connect(ui.actionMonitoring, &QAction::triggered, [&]() {
        // Chiusura analisi

        // Chiusura DOCK analisi
        ui.monitoringManagerDock->setVisible(true);
        ui.lastMacDock->setVisible(true);
        ui.actionMonitoringDock->setEnabled(true);
        ui.actionlastMacDock->setEnabled(true);

        // Apri Dock Analisi
        ui.macDetectedDock->setVisible(false);
        ui.rangeSelectorDock->setVisible(false);
        ui.actionMacSituationDock->setEnabled(false);
        ui.actionRangeDock->setEnabled(false);

        // Disabilita azione Monitoring e abilita Analisi
        ui.actionAnalysis->setVisible(true);
        ui.actionMonitoring->setVisible(false);

        ui.title->setText(Styles::HEADER.arg(Strings::MON));

        // Cambia plot
        ui.monitoringWidget->setVisible(true);
        ui.analysisWidget->setVisible(false);
        setupMonitoringPlot();
        setupLivePlot();
        initializeLastMacList();
    });

    // Conseguenze Click Start Button
    QObject::connect(&s, &MonitoringServer::started, [&]() { s.start(); });

    // Click Stop Button
    QObject::connect(ui.stopButton, &QPushButton::clicked, [&]() {
        if (s.isRunning()) {
            this->stopServer();
        }
    });

    // Conseguenze Click Stop Button
    QObject::connect(&s, &MonitoringServer::stopped, [&]() {
        this->stopServer();
    });

    // Azione Impostazioni
    QObject::connect(ui.actionSettings, &QAction::triggered, [&]() {
        SettingDialog sd{};
        int ret = 0;
        if (s.isRunning() && Utility::yesNoMessage(this, Strings::ANA_RUNNING, Strings::ANA_RUNNING_MSG)) {
            s.stop();
            ui.startButton->setDisabled(false);
            ui.stopButton->setDisabled(true);
            sd.setModal(true);
            ret = sd.exec();
        } else if (!s.isRunning()) {
            sd.setModal(true);
            ret = sd.exec();
        }
        if (ret) {
            ui.actionMonitoring->triggered();
        }
    });

    // Azione Localizza MAC
    QObject::connect(ui.localizeButton, &QPushButton::clicked, [&]() {
        QString mac = ui.macSituation->selectedItems().at(0)->text();
        QDialog localize{};
        positionDialog.setupUi(&localize);
        ui.macSituation->selectedItems().first()->row();
        positionDialog.macLabel->setText(mac);
        setupPositionPlot(mac);
        localize.setModal(true);
        localize.exec();

    });

    // Azione Analisi MAC randomico
    QObject::connect(ui.randomButton, &QPushButton::clicked, [&]() {
        QString mac = ui.macSituation->selectedItems().at(0)->text();
        QDialog random{};
        randomDialog.setupUi(&random);
        auto macChart = new MacChart();
        macChart->fillRandomChart(
                this->getHiddenMacFor(mac, ui.startDate->dateTime(), ui.endDate->dateTime()));
        randomDialog.macLabel->setText(mac);
        randomDialog.macPlot->setChart(macChart);
        randomDialog.macPlot->setCalloutText("%1\n%2%");
        random.setModal(true);
        random.exec();

    });

    // Azione Informazioni Sviluppatori
    QObject::connect(ui.actionAbout, &QAction::triggered, [&]() {
        QDialog sd{};
        Ui::AboutDialog a{};
        a.setupUi(&sd);
        sd.setModal(true);
        sd.exec();

    });


}


/**
 * Stessa cosa della funzione getHiddenDeviceFor ma questa volta partendo solo dalla stringa del mac e non da un intero pacchetto
 * @param mac
 * @param initTime
 * @param endTime
 * @return
 */
QList<Statistic> MainWindow::getHiddenMacFor(QString mac, QDateTime initTime, QDateTime endTime) {
    QSettings su{Utility::ORGANIZATION, Utility::APPLICATION};
    QMap<QString, QList<qreal>> map_mac_stat;
    QList<Statistic> list{};
    // entro 5 minuti, stessa posizione +-0.5, altro da vedere
    // usata per definire entro quanto la posizione deve essere uguale, 240= 4 minuti
    uint32_t tolleranzaTimestamp = su.value("mac/time/tol").toInt();
    double tolleranzaDist = su.value("mac/pos/tol").toInt();
    bool checkTime = su.value("mac/time/check").toBool();
    bool checkDist = su.value("mac/pos/check").toBool();
    bool checkSsid = su.value("mac/ssid/check").toBool();
    int pesoTime = su.value("mac/time/peso").toInt();
    int pesoDist = su.value("mac/pos/peso").toInt();
    int pesoSsid = su.value("mac/ssid/peso").toInt();

    qreal perc;
    // Ottiene tutti i pacchetti nascosti in quel determinato intervallo
    std::deque<Packet> hiddenPackets = getHiddenPackets(initTime, endTime, mac);
    // Nel lasso di tempo scelto non è stato trovato nessun pacchetto.
    // Per effettuare la stima è necessario che ci sia almeno un pacchetto.
    if (hiddenPackets.empty())
        return list;

    // Ottiene tutti i pacchetti nascosti in quel determinato intervallo per MAC specificato
    std::list<Packet> allPacketsOfMac = getAllPacketsOfMac(mac, initTime, endTime);
    // Nel lasso di tempo scelto non è stato trovato nessun pacchetto con il MAC specificato.
    // Per effettuare la stima è necessario che ci sia almeno un pacchetto.
    if (allPacketsOfMac.empty())
        return list;

    for (auto &packet : hiddenPackets) {
        int common_ssid = 0;
        perc = 0;
        //controlla tutti i mac nascosti con tutte le posizioni del mac scelto nell'intervallo selezionato
        for (auto &source:allPacketsOfMac) {
            double diff = (source.getTimestamp() < packet.getTimestamp()) ? (
                    packet.getTimestamp() - source.getTimestamp()) : (source.getTimestamp() -
                                                                      packet.getTimestamp());
            double diffDist = packet.getPoint().distance(source.getPoint());
            //mac diverso ad intervallo inferiore di 1 minuto
            double percDist = (tolleranzaDist - diffDist < 0) ? 0 : (tolleranzaDist - diffDist) / tolleranzaDist;
            double percTime = (tolleranzaTimestamp - diff < 0) ? 0 : (tolleranzaTimestamp - diff) /
                                                                     tolleranzaTimestamp;
            double ssid = (source.getSsid() != "Nan" && source.getSsid() == packet.getSsid()) ? 1 : 0;

            // Calcolo Pesi
            int totalePesi = pesoDist * checkDist + pesoTime * checkTime + pesoSsid * checkSsid;
            perc += (percDist * pesoDist + percTime * pesoTime + ssid * pesoSsid) / totalePesi;
        }
        perc = perc / allPacketsOfMac.size();
        QString mac_hidden = QString::fromStdString(packet.getMacPeer());
        if (map_mac_stat.contains(mac_hidden)) {
            map_mac_stat.find(mac_hidden).value().push_back(perc);
        } else {
            QList<qreal> new_list{perc};
            map_mac_stat.insert(mac_hidden, new_list);
        }
    }
    for (auto it = map_mac_stat.begin(); it != map_mac_stat.end(); it++) {
        qreal final_perc = 0;
        for (auto &ot : it.value()) {
            final_perc += ot;
        }

        final_perc = final_perc / it.value().size();
        if (final_perc != 0.0) {
            Statistic s{it.key(), final_perc * 100};
            list.append(s);
        }
    }
    return list;
}

std::list<Packet> MainWindow::getAllPacketsOfMac(const QString &mac, QDateTime initTime, QDateTime endTime) {
    QSettings su{Utility::ORGANIZATION, Utility::APPLICATION};
    bool error = false;
    QString table = su.value("database/table").toString();
    QSqlDatabase db = Utility::getDB(error);
    if (error) exit(-1);
    QSqlQuery query{db};
    QDateTime timeInit;
    QDateTime timeEnd;

    query.prepare(Query::SELECT_HIDDEN_MAC.arg(table));
    query.bindValue(":fd", initTime.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":sd", endTime.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":mac", mac);

    if (!query.exec()) {
        //qDebug() << query.lastError();
        Utility::warningMessage(Strings::ERR_DB,
                                Strings::ERR_DB_MSG,
                                query.lastError().text());
        return std::list<Packet>{};
    }

    if (query.size() == 0) {
        return std::list<Packet>();
    }

    std::list<Packet> allPacketsOfMac;
    while (query.next()) {
        std::string fcs = query.value(1).toString().toStdString();
        std::string mac_add = query.value(2).toString().toStdString();
        uint32_t timestamp = query.value(5).toDateTime().toSecsSinceEpoch();
        std::string ssid = query.value(6).toString().toStdString();

        Packet p(-1, fcs, -1, mac_add, timestamp, ssid);

        double_t posX = query.value(3).toDouble();
        double_t posY = query.value(4).toDouble();
        PositionData positionData(posX, posY);
        p.setPosition(positionData);

        allPacketsOfMac.push_back(p);
    }

    db.close();

    return allPacketsOfMac;

}


std::deque<Packet> MainWindow::getHiddenPackets(QDateTime initTime, QDateTime endTime, QString &mac) {
    QSettings su{Utility::ORGANIZATION, Utility::APPLICATION};
    //query per ottenere i pacchetti con mac hidden nel periodo specificato
    QDateTime timeInit;
    QDateTime timeEnd;
    std::deque<Packet> hiddenPackets;
    QString table = su.value("database/table").toString();
    bool error = false;
    QSqlDatabase db = Utility::getDB(error);
    if (error) return std::deque<Packet>{};
    QSqlQuery query{db};
    query.prepare(Query::SELECT_HIDDEN_NOT_MAC.arg(table));
    query.bindValue(":fd", initTime.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":sd", endTime.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":mac", mac);
    if (!query.exec()) {
        //qDebug() << query.lastError();
        Utility::warningMessage(Strings::ERR_DB,
                                Strings::ERR_DB_MSG,
                                query.lastError().text());
        return std::deque<Packet>{};
    }
    QSqlRecord record = query.record();
    if (query.size() == 0)
        qDebug() << "Nessun risultato";

    while (query.next()) {                                //ciclo su ogni entry selezionata del db
        std::string fcs = query.value(1).toString().toStdString();
        std::string mac = query.value(2).toString().toStdString();
        uint32_t timestamp = query.value(5).toDateTime().toSecsSinceEpoch();
        std::string ssid = query.value(6).toString().toStdString();

        Packet p(-1, fcs, -1, mac, timestamp, ssid);

        double_t posX = query.value(3).toDouble();
        double_t posY = query.value(4).toDouble();
        PositionData positionData(posX, posY);
        p.setPosition(positionData);
        hiddenPackets.push_back(p);

    }
    return hiddenPackets;
}

void MainWindow::setupMonitoringPlot() {
    auto monitoringChart = new MonitoringChart();
    ui.monitoringPlot->setChart(monitoringChart);
}

void MainWindow::setupLivePlot() {
    auto liveChart = new LiveChart();
    ui.livePlot->setChart(liveChart);

    /** PLOT BOARDS **/
    std::vector<Board> boards = Utility::getBoards();
    liveChart->fillBoards(boards);
    liveChart->fillDevices(lastMacs);

}

void MainWindow::setupMapPlot() {
    auto liveChart = new LiveChart();
    ui.mapPlot->setChart(liveChart);
    ui.mapSlider->setSliderPosition(0);
    ui.mapSlider->setDisabled(true);

    /** PLOT BOARDS **/
    std::vector<Board> boards = Utility::getBoards();
    liveChart->fillBoards(boards);
    connect(ui.mapSlider, &QMapSlider::initialized, [&]() {
        if (ui.mapSlider->isMapEmpty()) {
            ui.dateTimePlot->setText("Nessun Dato");
        } else {
            ui.dateTimePlot->setText(ui.mapSlider->getKeyIndex(0).toString("dd/MM/yyyy hh:mm"));
            ui.mapPlot->getChart()->fillDevicesV(ui.mapSlider->getMapIndex(0));
        }
    });
    connect(ui.mapSlider, &QSlider::valueChanged, [&](int value) {
        if (value < 0 || ui.mapSlider->isMapEmpty()) return;
        ui.mapPlot->getChart()->fillDevicesV(ui.mapSlider->getMapIndex(value));
        ui.dateTimePlot->setText(ui.mapSlider->getKeyIndex(value).toString("dd/MM/yyyy hh:mm"));
    });

}

void MainWindow::setupAnalysisPlot() {
    auto monitoringChart = new MonitoringChart();
    // Plot Analysis Chart
    ui.analysisPlot->setChart(monitoringChart);
    setMacPlot();
    setupMapPlot();
}

void MainWindow::initializeMacSituationList() {
    ui.macSituation->reset();
    ui.macSituation->setRowCount(0);
    /* SETUP TABLE */
    ui.macSituation->setColumnCount(3);
    QStringList h;
    h << "MAC" << "Frequenza" << "Random";
    ui.macSituation->setHorizontalHeaderLabels(h);
    ui.macSituation->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.macSituation->verticalHeader()->hide();
    ui.macSituation->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.macSituation->setSelectionMode(QHeaderView::SelectionMode::SingleSelection);
    ui.macSituation->setAlternatingRowColors(true);
    ui.macSituation->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // Connect
    connect(ui.macSituation, &QTableWidget::itemSelectionChanged, this, [&]() {
        auto i = ui.macSituation->selectedItems();
        if (i.size() == 0) {
            ui.randomButton->setEnabled(false);
            ui.localizeButton->setEnabled(false);
            return;
        }
        bool ran = i.at(2)->text() == "true";
        {
            ui.randomButton->setEnabled(ran);
            ui.localizeButton->setEnabled(true);
        }
    });
}

void MainWindow::addMacSitua(const QString &mac, int frequency, bool random) {
    int i = ui.macSituation->rowCount();
    ui.macSituation->insertRow(i);
    auto mac_table = new QTableWidgetItem{mac};
    mac_table->setToolTip(mac);
    auto frequency_table = new QTableWidgetItem{QString::number(frequency)};
    auto random_table = new QTableWidgetItem{random ? "true" : "false"};

    mac_table->setToolTip(mac);
    ui.macSituation->setItem(i, 0, mac_table);
    ui.macSituation->setItem(i, 1, frequency_table);
    ui.macSituation->setItem(i, 2, random_table);

}

void MainWindow::initializeLastMacList() {
    ui.macLastSituation->reset();
    /* SETUP TABLE */
    ui.macLastSituation->setColumnCount(4);
    ui.macLastSituation->setRowCount(0);
    QStringList h;
    h << "MAC" << "Time" << "X" << "Y";
    ui.macLastSituation->setHorizontalHeaderLabels(h);
    ui.macLastSituation->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.macLastSituation->verticalHeader()->hide();
    ui.macLastSituation->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.macLastSituation->setSelectionMode(QHeaderView::SelectionMode::SingleSelection);
    ui.macLastSituation->setAlternatingRowColors(true);
    ui.macLastSituation->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void MainWindow::addLastMacPos(const QString &mac, const QDateTime &date, qreal posx, qreal posy) {
    int i = ui.macLastSituation->rowCount();
    ui.macLastSituation->insertRow(i);

    auto mac_table = new QTableWidgetItem{mac};
    auto date_table = new QTableWidgetItem{date.toString("dd/MM/yyyy hh:mm")};
    auto posx_table = new QTableWidgetItem{QString::number(posx)};
    auto posy_table = new QTableWidgetItem{QString::number(posy)};
    mac_table->setToolTip(mac);

    ui.macLastSituation->setItem(i, 0, mac_table);
    ui.macLastSituation->setItem(i, 1, date_table);
    ui.macLastSituation->setItem(i, 2, posx_table);
    ui.macLastSituation->setItem(i, 3, posy_table);
}


void MainWindow::setMacPlot() {
    auto macPlot = new MacChart();
    QVector<MacOccurrence> macs;
    macPlot->fillChart(macs);
    ui.macPlot->setChart(macPlot);
}

void MainWindow::setupPositionPlot(QString mac) {
    QSettings su{Utility::ORGANIZATION, Utility::APPLICATION};
    auto posPlot = new PositionPlot();// Plot Analysis Chart
    positionDialog.positionPlot->setChart(posPlot);
    QDateTime start_in = ui.startDate->dateTime();
    QDateTime end_in = ui.endDate->dateTime();
    bool error = false;
    QSqlDatabase db = Utility::getDB(error);
    if (error) return;
    QSqlQuery query{db};
    query.prepare(Query::SELECT_MAC_TIMING_AVGPOS_SINGLE.arg(su.value("database/table").toString()));
    query.bindValue(":fd", start_in.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":sd", end_in.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":mac", mac);

    if (!query.exec()) {
        Utility::warningMessage(Strings::ERR_DB,
                                Strings::ERR_DB_MSG,
                                db.lastError().text());
        db.close();
        return;
    }

    std::vector<PositionDataPlot> v;
    while (query.next()) {
        QDateTime timing = query.value(1).toDateTime();
        qreal posx = query.value(2).toDouble();
        qreal posy = query.value(3).toDouble();
        PositionDataPlot p{timing, posx, posy};
        v.push_back(p);
    }
    posPlot->fillData(v);
    positionDialog.horizontalSlider->setData(v);
    positionDialog.horizontalSlider->setRange(0, static_cast<int>(v.size()) - 1);
    positionDialog.startDate->setText(v[0].getData().toString("yyyy-MM-dd hh:mm:ss"));
    positionDialog.startPos->setText("(" + QString::number(v[0].getX()) + ", " + QString::number(v[0].getY()) + ")");
    positionDialog.endDate->setText(v[0].getData().toString("yyyy-MM-dd hh:mm:ss"));
    positionDialog.endPos->setText("(" + QString::number(v[0].getX()) + ", " + QString::number(v[0].getY()) + ")");

    // FINE
    connect(ui.mapSlider, &QMapSlider::initialized, [&]() {
        ui.dateTimePlot->setText(ui.mapSlider->getKeyIndex(0).toString("dd/MM/yyyy hh:mm"));
        ui.mapPlot->getChart()->fillDevicesV(ui.mapSlider->getMapIndex(0));
    });
    connect(ui.mapSlider, &QSlider::valueChanged, [&](int value) {
        ui.mapPlot->getChart()->fillDevicesV(ui.mapSlider->getMapIndex(value));
        ui.dateTimePlot->setText(ui.mapSlider->getKeyIndex(value).toString("dd/MM/yyyy hh:mm"));
    });

    connect(positionDialog.horizontalSlider, &QSlider::valueChanged, posPlot, &PositionPlot::sliderChanged);
    connect(posPlot, &PositionPlot::dataChanged, [&](PositionDataPlot e) {
        positionDialog.endDate->setText(e.getData().toString("yyyy-MM-dd hh:mm:ss"));
        positionDialog.endPos->setText("(" + QString::number(e.getX()) + ", " + QString::number(e.getY()) + ")");
    });
    connect(positionDialog.horizontalSlider, &QTimeSlider::emitDateMax, [&](PositionDataPlot e) {
        positionDialog.endDate->setText(e.getData().toString("yyyy-MM-dd hh:mm:ss"));
        positionDialog.endPos->setText("(" + QString::number(e.getX()) + ", " + QString::number(e.getY()) + ")");
    });

}

void MainWindow::addLiveData() {
    QSettings su{Utility::ORGANIZATION, Utility::APPLICATION};
    qint64 startTimestamp = QDateTime::currentSecsSinceEpoch();
    startTimestamp = startTimestamp / (60 * 5);
    QDateTime start{};
    QDateTime end{};
    QDateTime prev{};
    start.setSecsSinceEpoch(startTimestamp * 60 * 5);
    end = start.addSecs(60 * 5);
    prev = start.addSecs(-60 * 5);

    bool error = false;
    QSqlDatabase db = Utility::getDB(error);
    if (error) return;

    QSqlQuery query{db};

    query.prepare(Query::SELECT_COUNT_LIVE.arg(su.value("database/table").toString()));
    query.bindValue(":fd", prev.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":sd", start.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":sec", 300);
    query.bindValue(":freq", su.value("monitor/min").toInt());

    if (!query.exec()) {
        //qDebug() << query.lastError();
        Utility::warningMessage(Strings::ERR_DB,
                                Strings::ERR_DB_MSG,
                                db.lastError().text());
        return;
    }
    if (!query.first())
        ui.monitoringPlot->getChart()->updateData(prev, 0);
    else
        ui.monitoringPlot->getChart()->updateData(prev, query.value(0).toInt());


    query.clear();

    query.prepare(Query::SELECT_COUNT_LIVE.arg(su.value("database/table").toString()));
    query.bindValue(":fd", start.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":sd", end.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":sec", 300);
    query.bindValue(":freq", su.value("monitor/min").toInt());

    if (!query.exec()) {
        //qDebug() << query.lastError();
        Utility::warningMessage(Strings::ERR_DB,
                                Strings::ERR_DB_MSG,
                                query.lastError().text());
        return;
    }

    if (!query.first())
        ui.monitoringPlot->getChart()->addData(start, 0);
    else
        ui.monitoringPlot->getChart()->addData(start, query.value(0).toInt());

    query.clear();

    db.close();
}

void MainWindow::updateLastMac() {
    this->initializeLastMacList();
    for (const auto &i: lastMacs) {
        this->addLastMacPos(i.getMac(), i.getTiming(), i.getPosx(), i.getPosy());
    }
}

void MainWindow::dataAnalysis() {
    setupAnalysisPlot();
    auto chart = new MonitoringChart();
    auto macPlot = new MacChart();
    QSettings su{Utility::ORGANIZATION, Utility::APPLICATION};
    QDateTime start_in = ui.startDate->dateTime();
    QDateTime end_in = ui.endDate->dateTime();
    QDateTime start{};
    start.setSecsSinceEpoch(start_in.toSecsSinceEpoch() / (60 * 5) * (60 * 5));
    QDateTime end{};
    end.setSecsSinceEpoch(end_in.toSecsSinceEpoch() / (60 * 5) * (60 * 5));
    auto worker = new AnalysisWorker(start, end, chart, macPlot, ui.mapSlider);
    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::started, worker, &AnalysisWorker::doWork);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(worker, &AnalysisWorker::resultReady, this, &MainWindow::handleResults);
    connect(worker, &AnalysisWorker::updateProgress, this, &MainWindow::updateProgress);
    connect(worker, &AnalysisWorker::macPlotReady, this, &MainWindow::macPlotReady);
    connect(worker, &AnalysisWorker::initializeMacSituation, this, &MainWindow::initializeMacSituationList);
    connect(worker, &AnalysisWorker::addMac, this, &MainWindow::addMacSitua);
    connect(worker, &AnalysisWorker::finished, this, &MainWindow::finishedAnalysisThread);
    connect(worker, &AnalysisWorker::warning, this, &Utility::warningMessage);


    ui.searchButton->setEnabled(false);
    workerThread.start();
}

void MainWindow::genLiveData() {
    QSettings su{Utility::ORGANIZATION, Utility::APPLICATION};
    bool error = false;
    QSqlDatabase db = Utility::getDB(error);
    if (error) return;
    qint64 startTimestamp = QDateTime::currentSecsSinceEpoch();
    QDateTime start{};
    QDateTime prev{};
    start.setSecsSinceEpoch(startTimestamp);
    prev = start.addSecs(-60 * 5);
    QSqlQuery query{db};
    /** QUERY_3 **/
    query.prepare(Query::SELECT_MAC_TIMING_LASTPOS.arg(su.value("database/table").toString()));
    query.bindValue(":fd", prev.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":sd", start.toString("yyyy-MM-dd hh:mm:ss"));
    if (!query.exec()) {
        //qDebug() << query.lastError();
        Utility::warningMessage(Strings::ERR_DB,
                                Strings::ERR_DB_MSG,
                                query.lastError().text());

        return;
    }

    lastMacs.clear();
    while (query.next()) {
        QString mac = query.value(0).toString();
        QDateTime timing = query.value(1).toDateTime();
        qreal posx = query.value(2).toDouble();
        qreal posy = query.value(3).toDouble();
        auto it = lastMacs.find(mac);
        if (it != lastMacs.end()) {
            it->update(timing, posx, posy);
        } else {
            LastMac lm{mac, timing, posx, posy};
            lastMacs.insert(mac, lm);
        }
    }
    this->updateLastMac();
    std::vector<LastMac> v;
    this->setupLivePlot();

    db.close();

}

void MainWindow::handleResults(MonitoringChart *chart) {
    ui.analysisPlot->setChart(chart);
}

void MainWindow::updateProgress(qint64 prog) {
    ui.progressBar->setEnabled(true);
    ui.progressBar->setValue(prog);
}

void MainWindow::macPlotReady(QStringList mac, QStringList frequency, MacChart *macPlot) {
    QVector<MacOccurrence> macs{};
    for (int i = 0; i < mac.size(); i++) {
        MacOccurrence m{mac[i], frequency[i].toInt()};
        macs.append(m);
    }
    macPlot->fillChart(macs);
    ui.macPlot->setChart(macPlot);
}

void MainWindow::finishedAnalysisThread() {
    workerThread.quit();
    workerThread.wait();
    ui.progressBar->setValue(100);
    ui.progressBar->setEnabled(false);
    ui.searchButton->setEnabled(true);
    ui.mapSlider->setDisabled(false);
}

void MainWindow::stopServer() {
    ui.startButton->setDisabled(false);
    ui.stopButton->setDisabled(true);
    s.stop();
}
