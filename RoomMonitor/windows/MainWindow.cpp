//
// Created by pieromack on 11/09/19.
//

#include "MainWindow.h"
#include "SettingDialog.h"

MainWindow::MainWindow() {
    ui.setupUi(this);
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
