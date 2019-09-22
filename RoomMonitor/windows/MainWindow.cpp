//
// Created by pieromack on 11/09/19.
//

#include <windows/plots/mac/MacChart.h>
#include <windows/classes/MacOccurrence.h>
#include "MainWindow.h"
#include "SettingDialog.h"

MainWindow::MainWindow() {

    ui.setupUi(this);
    setupConnect();
    ui.actionMonitoring->triggered(true);
}

void MainWindow::setupConnect() {
    ui.stopButton->setDisabled(true);

    // Click Start Button

    QObject::connect(ui.startButton, &QPushButton::clicked, [&]() {
        try {
            // TODO: insert from settings
            int n = 2;
            if (n > 0) {
                s.started();
                ui.startButton->setDisabled(true);
                ui.stopButton->setDisabled(false);
            }
        } catch (std::exception &e) {
            // Does not started signal
            std::cout << "Non è stato possibile avviare il server." << std::endl;
            return;
        }
    });

    // Click Analysis Button

    QObject::connect(ui.actionAnalysis, &QAction::triggered, [&]() {
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
        s.stopped();
        ui.startButton->setDisabled(false);
        ui.stopButton->setDisabled(true);
        s.stop();


    });

    // Conseguenze Click Start Button

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

    QObject::connect(ui.localizeButton, &QPushButton::clicked, [&]() {
        // TODO: Implement Localize Dialog
        QDialog localize{};
        localize.setModal(true);
        localize.exec();

    });

    QObject::connect(ui.randomButton, &QPushButton::clicked, [&]() {
        // TODO: Implement Random Dialog
        QDialog random{};
        random.setModal(true);
        random.exec();

    });

    QObject::connect(ui.actionAbout, &QAction::triggered, [&]() {
        QDialog sd{};
        Ui::AboutDialog a{};
        a.setupUi(&sd);
        sd.setModal(true);
        sd.exec();

    });


}

void MainWindow::setupMonitoringPlot() {
    monitoringChart = new MonitoringChart();
    ui.monitoringPlot->setChart(monitoringChart);

    startTime.setDate(QDate(2019, 9, 18));
    startTime.setTime(QTime(10, 0, 0));
    for (int i = 0; i < 11; i++) {
        QDateTime momentInTime = startTime.addSecs(60 * 5 * i_time);
        i_time++;
        monitoringChart->addData(momentInTime, std::rand() % 20);
    }
    monitoringChart->updateData(startTime, 0);
//    connect(ui.aggiungidatiFittizi, &QPushButton::clicked, [&](){
//        QDateTime momentInTime = startTime.addSecs(60*5*i_time);
//        i_time++;
//        monitoringChart->addData(momentInTime, std::rand()%20);
//    });

}

void MainWindow::setupAnalysisPlot() {
    monitoringChart = new MonitoringChart();
    // Plot Analysis Chart
    ui.analysisPlot->setChart(monitoringChart);
//    ui.macPlot->setChart(monitoringChart);

    startTime.setDate(QDate(2019, 9, 18));
    startTime.setTime(QTime(10, 0, 0));
    for (int i = 0; i < 11; i++) {
        QDateTime momentInTime = startTime.addSecs(60 * 5 * i_time);
        i_time++;
        monitoringChart->addData(momentInTime, std::rand() % 20);
    }
    monitoringChart->updateData(startTime, 0);

    // TODO: Plot MAC occurrences
    setPlotMacOne();
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
    connect(ui.macSituation, &QTableWidget::itemSelectionChanged, this, [&](){
        auto i = ui.macSituation->selectedItems();
        if (i.size() == 0){
            ui.randomButton->setEnabled(false);
            ui.localizeButton->setEnabled(false);
            return;
        }
        bool ran = i.at(3)->text() == "true";
        if (ran){
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
    ui.macLastSituation->setColumnCount(3);
    ui.macLastSituation->setRowCount(0);
    QStringList h;
    h << "MAC" << "X" << "Y";
    ui.macLastSituation->setHorizontalHeaderLabels(h);
    ui.macLastSituation->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.macLastSituation->verticalHeader()->hide();
    ui.macLastSituation->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.macLastSituation->setSelectionMode(QHeaderView::SelectionMode::SingleSelection);
    ui.macLastSituation->setAlternatingRowColors(true);
    ui.macLastSituation->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // TODO: delete fake data
    addLastMacPos("BB:BB:BB:BB:BB:BB", 1.0, 3.0);
    addLastMacPos("CC:CC:CC:CC:CC:CC", 2.0, 0.0);
    addLastMacPos("AA:AA:AA:AA:AA:AA", 0.0, 2.5);
}

void MainWindow::addLastMacPos(const QString &mac, qreal posx, qreal posy) {
    int i = ui.macLastSituation->rowCount();
    ui.macLastSituation->insertRow(i);

    auto mac_table = new QTableWidgetItem{mac};
    auto posx_table = new QTableWidgetItem{QString::number(posx)};
    auto posy_table = new QTableWidgetItem{QString::number(posy)};
    mac_table->setToolTip(mac);

    ui.macLastSituation->setItem(i, 0, mac_table);
    ui.macLastSituation->setItem(i, 1, posx_table);
    ui.macLastSituation->setItem(i, 2, posy_table);

}


void MainWindow::setPlotMacOne() {
    macPlot = new MacChart();
    QVector<MacOccurrence> macs;
    for (int i = 0; i < 20; i++) {
        QString st {"MAC %1"};
        MacOccurrence m{st.arg(i), i};
        macs.append(m);
    }
    macPlot->fillChart(macs);
    ui.macPlot->setChart(macPlot);
}
