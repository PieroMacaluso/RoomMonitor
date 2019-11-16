#include <QApplication>
#include "monitoring/MonitoringServer.h"
#include "windows/MainWindow.h"
#include "windows/SettingDialog.h"
#include "Utility.h"

/**
 * Starting main
 */
int main(int argc, char **argv) {
    QApplication a{argc, argv};
    int ret;
    // Check settings, otherwise set them to default values
    if (!SettingDialog::settingCheckUp()) {
        QSettings su{Utility::ORGANIZATION,Utility::APPLICATION};
        su.remove("first_time");
        Utility::infoMessage("Primo Avvio", "Questo è il primo avvio dell'applicazione. Prima di continuare compila "
                                               "le impostazioni.");
        SettingDialog s{};
        if (!s.exec() && !SettingDialog::settingCheckUp()){
            return -1;
        }
    };
    MainWindow w;
    w.show();
    ret = QApplication::exec();
    return ret;
}
