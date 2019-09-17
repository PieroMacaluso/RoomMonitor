//
// Created by pieromack on 11/09/19.
//

#include "MainWindow.h"
#include "SettingDialog.h"

MainWindow::MainWindow() {
    ui.setupUi(this);
    setPlotMacOne();
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

void MainWindow::setPlotMacOne() {
    QBarSet *set0 = new QBarSet("Presenze");
    QHorizontalBarSeries *series = new QHorizontalBarSeries();
    series->append(set0);

    *set0 << 1 << 2 << 3 << 4 << 5 << 6 << 1 << 2 << 3 << 4 << 5 << 6;
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Numero di rilevazioni dei MAC presenti");
//    chart->setAnimationOptions(QChart::SeriesAnimations);
    // TODO: categories MAC
    QStringList categories;
    categories << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun" << "Jul" << "A" << "B" << "C" << "D" << "E" << "F" << "G";
    QBarCategoryAxis *axiY = new QBarCategoryAxis();
    axiY->append(categories);
    axiY->setRange(categories[0], categories[5]);
    chart->addAxis(axiY, Qt::AlignLeft);
    series->attachAxis(axiY);

    QValueAxis *axiX = new QValueAxis();
    axiX->setRange(0,15);
    chart->addAxis(axiX, Qt::AlignBottom);
    series->attachAxis(axiX);
    chart->legend()->setVisible(false);
    ui.plot1->setMouseTracking(true);
    ui.plot1->setChart(chart);
//    ui.plot1->setRenderHint(QPainter::Antialiasing);
    ui.plot1->setRubberBand(QChartView::VerticalRubberBand);
}
