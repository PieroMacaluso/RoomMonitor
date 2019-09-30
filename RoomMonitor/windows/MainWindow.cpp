//
// Created by pieromack on 11/09/19.
//

#include <windows/plots/mac/MacChart.h>
#include <windows/classes/MacOccurrence.h>
#include <windows/classes/PositionDataPlot.h>
#include "MainWindow.h"
#include "SettingDialog.h"

MainWindow::MainWindow() {
    ui.setupUi(this);
    setupConnect();
    QSettings su{"VALP", "RoomMonitoring"};
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(su.value("database/host").toString());
    db.setDatabaseName(su.value("database/name").toString());
    db.setPort(su.value("database/port").toInt());
    db.setUserName(su.value("database/user").toString());
    db.setPassword(su.value("database/pass").toString());
    ui.actionMonitoring->triggered(true);
}

void MainWindow::setupConnect() {
    ui.stopButton->setDisabled(true);

    connect(&liveGraph, &QTimer::timeout, this, &MainWindow::addLiveData);

    // Click Start Button
    QObject::connect(ui.startButton, &QPushButton::clicked, [&]() {
        try {
            // TODO: insert from settings
            int n = 2;
            if (n > 0) {
                s.started();
                ui.startButton->setDisabled(true);
                ui.stopButton->setDisabled(false);
                // Pulizia tabella Ultimi MAC
                lastMacs.clear();
                liveGraph.start(1000 /** 60 * 5*/);
                i_time = 0;
                setupMonitoringPlot();

            }
        } catch (std::exception &e) {
            // Does not started signal
            std::cout << "Non è stato possibile avviare il server." << std::endl;
            return;
        }
    });

    // Click Search Button
    QObject::connect(ui.searchButton, &QPushButton::clicked, [&]() {
        if (ui.startDate->dateTime().addSecs(60 * 5) > ui.endDate->dateTime()) {
            QMessageBox m{};
            m.setStandardButtons(QMessageBox::Close);
            m.setWindowTitle("Errore selezione range");
            m.setIcon(QMessageBox::Warning);
            m.setText(
                    "La data di inizio deve essere antecedente alla data di fine.\nLa loro differenza deve essere maggiore o uguale a 5 minuti.");
            m.exec();
            return;
        }

        // TODO: QUERY ricerca e popolamento grafici
    });

    // Click Analysis Button
    QObject::connect(ui.actionAnalysis, &QAction::triggered, [&]() {
        if (s.isRunning()) {
            // TODO: provare a togliere e testare
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Analisi in corso",
                                          "L'analisi in corso verrà interrotta. Continuare?",
                                          QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                qDebug() << "Yes was clicked";
            } else {
                qDebug() << "Yes was *not* clicked";
                return;
            }
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
        ui.monitoringPlot->setVisible(false);
        ui.analysisWidget->setVisible(true);

        ui.title->setText(
                "<html><head/><body><p><span style=\" font-size:22pt; font-weight:600;\">Analisi dei Dati</span></p></body></html>");
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

        ui.title->setText(
                "<html><head/><body><p><span style=\" font-size:22pt; font-weight:600;\">Monitoraggio Stanza</span></p></body></html>");

        // Cambia plot
        ui.monitoringPlot->setVisible(true);
        ui.analysisWidget->setVisible(false);
        setupMonitoringPlot();
        initializeLastMacList();

    });

    // Conseguenze Click Start Button
    QObject::connect(&s, &MonitoringServer::started, [&]() { s.start(); });

    // Click Stop Button
    QObject::connect(ui.stopButton, &QPushButton::clicked, [&]() {
        if (s.isRunning()) {
            s.stopped();
            ui.startButton->setDisabled(false);
            ui.stopButton->setDisabled(true);
            s.stop();
            liveGraph.stop();
        }
    });

    // Conseguenze Click Stop Button
    QObject::connect(&s, &MonitoringServer::stopped, [&]() {
        std::cout << "Stopped" << std::endl;
    });

    // Azione Impostazioni
    QObject::connect(ui.actionSettings, &QAction::triggered, [&]() {
        SettingDialog sd{};
        if (s.isRunning()) {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Analisi in corso",
                                          "L'analisi in corso verrà interrotta. Continuare?",
                                          QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                qDebug() << "Yes was clicked";
            } else {
                qDebug() << "Yes was *not* clicked";
                return;
            }
            s.stop();
            ui.startButton->setDisabled(false);
            ui.stopButton->setDisabled(true);
        }

        sd.setModal(true);
        sd.exec();

    });

    // Azione Localizza MAC
    QObject::connect(ui.localizeButton, &QPushButton::clicked, [&]() {
        // TODO: Implement Localize Dialog
        QString mac = ui.macSituation->selectedItems().at(0)->text();
        QDialog localize{};
        positionDialog.setupUi(&localize);
        positionDialog.macLabel->setText(mac);
        setupPositionPlot();
        localize.setModal(true);
        localize.exec();

    });

    // Azione Analisi MAC randomico
    QObject::connect(ui.randomButton, &QPushButton::clicked, [&]() {
        // TODO: Implement Random Dialog
        QDialog random{};
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

void MainWindow::setupMonitoringPlot() {
    auto monitoringChart = new MonitoringChart();
    ui.monitoringPlot->setChart(monitoringChart);

//    // TODO: dati fittizi da rimuovere alla fine
//    QDateTime startTime{};
//    startTime.setDate(QDate(2019, 9, 18));
//    startTime.setTime(QTime(10, 0, 0));
//    for (int i = 0; i < 11; i++) {
//        QDateTime momentInTime = startTime.addSecs(60 * 5 * i);
//        monitoringChart->addData(momentInTime, std::rand() % 20);
//    }
//    monitoringChart->updateData(startTime, 0);
    // Fine dati da rimuovere
}

void MainWindow::setupAnalysisPlot() {
    auto monitoringChart = new MonitoringChart();
    // Plot Analysis Chart
    ui.analysisPlot->setChart(monitoringChart);
//    ui.macPlot->setChart(monitoringChart);

    // TODO: dati fittizi da rimuovere alla fine
    QDateTime startTime{};
    startTime.setDate(QDate(2019, 9, 18));
    startTime.setTime(QTime(10, 0, 0));
    for (int i = 0; i < 11; i++) {
        QDateTime momentInTime = startTime.addSecs(60 * 5 * i);
        monitoringChart->addData(momentInTime, std::rand() % 20);
    }
    monitoringChart->updateData(startTime, 0);

    // TODO: Plot MAC occurrences
    setMacPlot();
//    startTime.setDate(QDate(2019, 9, 18));
//    startTime.setTime(QTime(10, 0, 0));
//    for (int i = 0; i < 11; i++) {
//        QDateTime momentInTime = startTime.addSecs(60 * 5 * i_time);
//        i_time++;
//        monitoringChart->addData(momentInTime, std::rand() % 20);
//    }
//    monitoringChart->updateData(startTime, 0);

}

void MainWindow::initializeMacSituationList() {
    ui.macSituation->reset();
    ui.macSituation->setRowCount(0);
    /* SETUP TABLE */
    ui.macSituation->setColumnCount(4);
    QStringList h;
    h << "MAC" << "X" << "Y" << "Random";
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
        bool ran = i.at(3)->text() == "true";
        if (ran) {
            ui.randomButton->setEnabled(true);
            ui.localizeButton->setEnabled(true);
        } else {
            ui.randomButton->setEnabled(false);
            ui.localizeButton->setEnabled(true);
        }
    });
    // TODO: delete fake data
    addMacSitua("AA:AA:AA:AA:AA:AA", 0, 2.5, true);
    addMacSitua("BB:BB:BB:BB:BB:BB", 1, 3, false);
    addMacSitua("CC:CC:CC:CC:CC:CC", 2, 0, false);
}

void MainWindow::addMacSitua(const QString &mac, qreal posx, qreal posy, bool random) {
    int i = ui.macSituation->rowCount();
    ui.macSituation->insertRow(i);
    auto mac_table = new QTableWidgetItem{mac};
    mac_table->setToolTip(mac);
    auto posx_table = new QTableWidgetItem{QString::number(posx)};
    auto posy_table = new QTableWidgetItem{QString::number(posy)};
    auto random_table = new QTableWidgetItem{random ? "true" : "false"};

    mac_table->setToolTip(mac);
    ui.macSituation->setItem(i, 0, mac_table);
    ui.macSituation->setItem(i, 1, posx_table);
    ui.macSituation->setItem(i, 2, posy_table);
    ui.macSituation->setItem(i, 3, random_table);

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
//    // TODO: Cancella dati fittizi alla fine
//    addLastMacPos("BB:BB:BB:BB:BB:BB", 1.0, 3.0);
//    addLastMacPos("CC:CC:CC:CC:CC:CC", 2.0, 0.0);
//    addLastMacPos("AA:AA:AA:AA:AA:AA", 0.0, 2.5);
    // FINE
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
    // TODO: rimuovi dati fittizi alla fine
    QVector<MacOccurrence> macs;
    for (int i = 0; i < 20; i++) {
        QString st{"MAC %1"};
        MacOccurrence m{st.arg(i), i};
        macs.append(m);
    }
    macPlot->fillChart(macs);
    // FINE
    ui.macPlot->setChart(macPlot);
}

void MainWindow::setupPositionPlot() {
    auto posPlot = new PositionPlot();// Plot Analysis Chart
    positionDialog.positionPlot->setChart(posPlot);
    // TODO: Cancella dati fittizi
    std::vector<PositionDataPlot> v;
    QDateTime startTime{};
    startTime.setDate(QDate(2019, 9, 18));
    startTime.setTime(QTime(10, 0, 0));
    for (int i = 0; i < 100; i++) {
        QDateTime momentInTime = startTime.addSecs(60 * 5 * i);
        PositionDataPlot p{momentInTime, (std::rand() % 10) * 1.0, (std::rand() % 10) * 1.0};
        v.push_back(p);
    }

    posPlot->fillData(v);
    positionDialog.horizontalSlider->setRange(0, v.size());
    // FINE
    connect(positionDialog.horizontalSlider, &QSlider::valueChanged, posPlot, &PositionPlot::sliderChanged);
}

void MainWindow::addLiveData() {
    QSettings su{"VALP", "RoomMonitoring"};
    qint64 startTimestamp = QDateTime::currentSecsSinceEpoch();
    // TODO: Decommenta queste due linee per testing. Da cancellare alla fine
    startTimestamp = 1569420000 + 60 * 5 * i_time;
    i_time++;
    startTimestamp = startTimestamp / (60 * 5);
    QDateTime start{};
    QDateTime end{};
    QDateTime prev{};
    start.setSecsSinceEpoch(startTimestamp * 60 * 5);
    end = start.addSecs(60 * 5);
    prev = start.addSecs(-60 * 5);

    QSqlDatabase db = QSqlDatabase::database();
    if (!db.open()) {
        qDebug() << db.lastError();
        return;
    }

    QSqlQuery query{};

    query.prepare(
            "SELECT COUNT(DISTINCT mac_addr) FROM " + su.value("database/table").toString() +
            " WHERE timestamp BETWEEN :fd AND :sd;");
    query.bindValue(":fd", prev.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":sd", start.toString("yyyy-MM-dd hh:mm:ss"));
    if (!query.exec())
        qDebug() << query.lastError();

    if (!query.first())
        ui.monitoringPlot->getChart()->updateData(prev, 0);
    else
        ui.monitoringPlot->getChart()->updateData(prev, query.value(0).toInt());


    query.clear();

    query.prepare("SELECT COUNT(DISTINCT mac_addr) FROM " + su.value("database/table").toString() +
                  " WHERE timestamp BETWEEN :fd AND :sd;");
    query.bindValue(":fd", start.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":sd", end.toString("yyyy-MM-dd hh:mm:ss"));
    if (!query.exec())
        qDebug() << query.lastError();

    if (!query.first())
        ui.monitoringPlot->getChart()->addData(start, 0);
    else
        ui.monitoringPlot->getChart()->addData(start, query.value(0).toInt());

    query.clear();

    query.prepare(
            "SELECT mac_addr, FROM_UNIXTIME(UNIX_TIMESTAMP(timestamp) - MOD(UNIX_TIMESTAMP(timestamp), 300)) AS timing, avg(pos_x) AS pos_x ,avg(pos_y)AS pos_x FROM eurecomLab WHERE timestamp BETWEEN :fd AND :sd GROUP BY mac_addr, UNIX_TIMESTAMP(timestamp) DIV :sec ORDER BY timing;");
    query.bindValue(":fd", prev.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":sd", start.toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":sec", 300);
    if (!query.exec())
        qDebug() << query.lastError();

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


    db.close();
}

void MainWindow::updateLastMac() {
    this->initializeLastMacList();
    for (const auto &i: lastMacs) {
        this->addLastMacPos(i.getMac(), i.getTiming(), i.getPosx(), i.getPosy());
    }
}
