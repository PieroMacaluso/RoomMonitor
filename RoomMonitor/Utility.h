//
// Created by pieromack on 01/10/19.
//

#ifndef ROOMMONITOR_UTILITY_H
#define ROOMMONITOR_UTILITY_H


#include <QtSql>
#include <QObject>
#include <QtCore/QSettings>
#include <QDebug>
#include <QThread>
#include <QtCharts>
#include "Strings.h"
#include <vector>
#include <monitoring/Board.h>
#include "Query.h"
#include <deque>
#include <monitoring/Packet.h>


class Utility {
public:
    static void setupVariables();
    static const QString ORGANIZATION;
    static const QString APPLICATION;
    static QSqlDatabase getDB(bool &error);
    static QLineSeries * generateRoomSeries(QObject *parent = nullptr);
    static void warningMessage(const QString &title, const QString &text, const QString &error);
    static void infoMessage(const QString &title, const QString &text);
    static int infoMessageTimer(const QString &title, const QString &text, int millisec);
    static bool yesNoMessage(QWidget *parent, const QString &title, const QString &text);

    static bool testTable(QSqlDatabase &db);

    static std::vector<Board> getBoards();

    static void dropBoards();

    static int RETRY_STEP_BOARD;
    static QString LOG_DIR;
    static QString LOG_FILE;

    /**
     * Funzione utilizzata per convertire il vettore di stringhe ricevute dalla esp in oggetti packet
     * @param packets
     * @return
     */
    static std::deque<Packet> string2packet(const std::vector<std::string> &p);

    template<class Container>
    static void split(const std::string &str, Container &cont, char delim);
};


#endif //ROOMMONITOR_UTILITY_H
