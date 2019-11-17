#include <QApplication>
#include "monitoring/MonitoringServer.h"
#include "windows/MainWindow.h"
#include "windows/SettingDialog.h"
#include "Utility.h"
#include <QtGlobal>

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

/**
 * Starting main
 */
int main(int argc, char **argv) {
    qInstallMessageHandler(myMessageOutput);
    QApplication a{argc, argv};
    int ret;
    // Check settings, otherwise set them to default values
    qInfo() << "RoomMonitor avviato";
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

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QString fileN;
    QString dirN;

    fileN = "log_" + Utility::LOG_FILE + ".txt";
    dirN = Utility::LOG_DIR;

    const QFileInfo outputDir(dirN);
    if ((!outputDir.exists()) || (!outputDir.isDir()) || (!outputDir.isWritable())) {
        fprintf(stderr, "output directory does not exist, is not a directory, or is not writeable\n");
        QDir().mkdir(dirN);
    }

    QFile file(dirN + "/" + fileN);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        fprintf(stderr, "Error log file\n");
        return;
    }

    QTextStream stream(&file);

    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
        case QtDebugMsg:
            stream << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "] " << "Debug:\t'"
                   << localMsg.constData() << "'" << endl;
            fprintf(stderr, "Debug: %s\n", localMsg.constData());
            //fprintf(stderr,"(%s:%u, %s)\n", context.file, context.line, context.function);
            break;
        case QtInfoMsg:
            stream << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "] " << "Info:\t\t'" << localMsg.constData()
                   << "'" << endl;
            fprintf(stderr, "Info: %s\n", localMsg.constData());
            //fprintf(stderr,"(%s:%u, %s)\n", context.file, context.line, context.function);
            break;
        case QtWarningMsg:
            stream << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "] " << "Warning:\t'"
                   << localMsg.constData() << "'" << endl;
            fprintf(stderr, "Warning: %s\n", localMsg.constData());
            //fprintf(stderr,"(%s:%u, %s)\n", context.file, context.line, context.function);
            break;
        case QtCriticalMsg:
            stream << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "] " << "Critical:\t'"
                   << localMsg.constData() << "'" << endl;
            fprintf(stderr, "Critical: %s\n", localMsg.constData());
            fprintf(stderr, "(%s:%u, %s)\n", context.file, context.line, context.function);
            break;
        case QtFatalMsg:
            stream << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "] " << "Fatal:\t'"
                   << localMsg.constData() << "'" << endl;
            fprintf(stderr, "Fatal: %s\n", localMsg.constData());
            fprintf(stderr, "(%s:%u, %s)\n", context.file, context.line, context.function);
            abort();
    }
}
