//
// Created by pieromack on 11/09/19.
//

#include "MainWindow.h"
#include "SettingDialog.h"

void addMacPos(QString mac, qreal posx, qreal posy);

MainWindow::MainWindow() {
    ui.setupUi(this);
    setupMonitoringPlot();
    initializeMacList();
    setupConnect();
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
       // TODO: Apertura finestra Analisi
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
        }
        s.stop();
        ui.startButton->setDisabled(false);
        ui.stopButton->setDisabled(true);
        sd.setModal(true);
        sd.exec();

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

    startTime.setDate(QDate(2019,9,18));
    startTime.setTime(QTime(10,0,0));
    for (int i = 0; i < 11; i++) {
        QDateTime momentInTime = startTime.addSecs(60*5*i_time);
        i_time++;
        monitoringChart->addData(momentInTime, std::rand()%20);
    }
    monitoringChart->updateData(startTime, 0);
//    connect(ui.aggiungidatiFittizi, &QPushButton::clicked, [&](){
//        QDateTime momentInTime = startTime.addSecs(60*5*i_time);
//        i_time++;
//        monitoringChart->addData(momentInTime, std::rand()%20);
//    });

}


void MainWindow::initializeMacList() {
    /* SETUP TABLE */
    ui.macSituation->setColumnCount(3);
    QStringList h;
    h << "MAC" << "X" << "Y";
    ui.macSituation->setHorizontalHeaderLabels(h);
    ui.macSituation->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.macSituation->verticalHeader()->hide();
    ui.macSituation->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.macSituation->setSelectionMode(QHeaderView::SelectionMode::SingleSelection);
    ui.macSituation->setAlternatingRowColors(true);
    ui.macSituation->setEditTriggers(QAbstractItemView::NoEditTriggers);
    addMacPos("AA:AA:AA:AA:AA:AA", 0, 2.5);
    addMacPos("BB:BB:BB:BB:BB:BB", 1, 3);
    addMacPos("CC:CC:CC:CC:CC:CC", 2, 0);
}
void MainWindow::addMacPos(QString mac, qreal posx, qreal posy){
        int i = ui.macSituation->rowCount();
        ui.macSituation->insertRow(ui.macSituation->rowCount());
        ui.macSituation->setItem(i, 0, new QTableWidgetItem(mac));
        ui.macSituation->setItem(i, 1, new QTableWidgetItem(QString::number(posx)));
        ui.macSituation->setItem(i, 2, new QTableWidgetItem(QString::number(posy)));
        ui.macSituation->resizeColumnsToContents();

}

