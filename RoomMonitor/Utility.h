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
#include "Query.h"
#include <deque>
#include "monitoring/Packet.h"
#include <stdexcept>
#include "monitoring/Board.h"


class Utility {
public:
    /**
     * Setup Variables
     */
    static void setupVariables();

    /**
     * Strings useful for the settings of the whole application
     */
    static const QString ORGANIZATION;
    static const QString APPLICATION;

    /**
     * Retry Step until warning message
     */
    static int RETRY_STEP_BOARD;

    /**
     * Directory of LOG
     */
    static QString LOG_DIR;

    /**
     * File of LOG
     */
    static QString LOG_FILE;

    /**
     * Function Standalone to get the SQL Database. It needs to pass a bool to check the correctnes in the opening of
     * the database
     * @param error
     * @return
     */
    static QSqlDatabase getDB(bool &error);

    /**
     * Generate the series of the room to plot
     * @param parent
     * @return
     */
    static QLineSeries *generateRoomSeries(QObject *parent = nullptr);

    /**
     * Function useful to display a Warning messages
     * @param title     Title of the window
     * @param text      Text of the window
     * @param error     Details of the error
     */
    static void warningMessage(const QString &title, const QString &text, const QString &error);

    /**
     * Function useful to display a info message
     * @param title     Title of the window
     * @param text
     */
    static void infoMessage(const QString &title, const QString &text);

    /**
     * Function prompts a message where the user can answer with yes or no
     * @param parent    Widget parent
     * @param title     Window Title
     * @param text      Window Text
     * @return
     */
    static bool yesNoMessage(QWidget *parent, const QString &title, const QString &text);

    /**
     * Function to test that the table is working
     * @param db    Database
     * @return bool     true if all ok, false otherwise
     */
    static bool testTable(QSqlDatabase &db);

    /**
     * Get Boards from DB
     * @return Board    vector of boards
     */
    static std::vector<Board> getBoards();

    /**
     * Delete Board table
     */
    static void dropBoards();

    /**
     * Funzione utilizzata per convertire il vettore di stringhe ricevute dalla esp in oggetti packet
     * @param packets
     * @return
     */
    static std::deque<Packet> string2packet(const std::vector<std::string> &p, const QList<int> &l);

    /**
     * Splitter String
     * @tparam Container    Container
     * @param str String to split
     * @param cont Container
     * @param delim Delimiter
     */
    template<class Container>
    static void split(const std::string &str, Container &cont, char delim);

    /**
     * Check if the device is inside the room
     * @param posx      Position X
     * @param posy      Position Y
     * @return true if it is inside, false otherwise
     */
    static bool is_inside_room(qreal posx, qreal posy);

};


#endif //ROOMMONITOR_UTILITY_H
