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
    qInfo() << Strings::RM_STARTED;
    if (!SettingDialog::settingCheckUp()) {
        QSettings su{Utility::ORGANIZATION,Utility::APPLICATION};
        su.remove("first_time");
        SettingDialog s{};
        Utility::infoMessage(Strings::FIRST_START, Strings::FIRST_START_MSG);
        if (!s.exec() && !SettingDialog::settingCheckUp()){
            return -1;
        }
    };
    MainWindow w;
    w.show();
    ret = QApplication::exec();
    qInfo() << Strings::RM_STOPPED;
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
    QTextStream stdOut(stdout);
    QTextStream stdErr(stderr);


    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
        case QtDebugMsg:
            /*stream << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "] " << "[DEBUG]\t'"
                   << localMsg.constData() << "'" << endl;*/
            fprintf(stdout, "Debug: %s\n", localMsg.constData());
            //fprintf(stderr,"(%s:%u, %s)\n", context.file, context.line, context.function);
            break;
        case QtInfoMsg:
            stream << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "] " << "[INFO]\t\t"
                   << localMsg.constData() << endl;
            stdOut << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "] " << "[INFO]\t\t"
                   << localMsg.constData() << endl;
            //fprintf(stderr,"(%s:%u, %s)\n", context.file, context.line, context.function);
            break;
        case QtWarningMsg:
            stream << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "] " << "[WARNING]\t\t"
                   << localMsg.constData() << endl;
            stdErr << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "] " << "[WARNING]\t\t"
                   << localMsg.constData() << endl;
            //fprintf(stderr,"(%s:%u, %s)\n", context.file, context.line, context.function);
            break;
        case QtCriticalMsg:
            stream << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "] " << "[CRITICAL]\t"
                   << localMsg.constData() << endl;
            stdErr << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "] " << "[CRITICAL]\t"
                   << localMsg.constData() << endl;
            fprintf(stderr, "(%s:%u, %s)\n", context.file, context.line, context.function);
            break;
        case QtFatalMsg:
            stream << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "] " << "[FATAL]\t\t"
                   << localMsg.constData() << endl;
            stdErr << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "] " << "[FATAL]\t"
                   << localMsg.constData() << endl;
            fprintf(stderr, "(%s:%u, %s)\n", context.file, context.line, context.function);
            abort();
    }
}
