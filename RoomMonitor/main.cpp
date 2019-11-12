#include <QApplication>
#include "monitoring/MonitoringServer.h"
#include "windows/MainWindow.h"
#include "windows/SettingDialog.h"

/**
 * Starting main
 */
int main(int argc, char **argv) {
    QApplication a{argc, argv};
    int ret;
    // Check settings, otherwise set them to default values
    SettingDialog::settingCheckUp();
    MainWindow w;
    w.show();
    ret = QApplication::exec();
    return ret;
}
