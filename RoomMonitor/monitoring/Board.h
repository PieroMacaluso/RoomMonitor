#ifndef ROOMMONITOR_BOARD_H
#define ROOMMONITOR_BOARD_H


#include "Point2d.h"
#include <QtCore/QSettings>


class Board {
    /**
     * ID Schedina
     */
    int id;

    /**
     * Coordinate schedina
     */
    Point2d coord;

    /**
     * dBm a un metro della schedina
     */
    int A;
public:
    /**
     * Costruttore Scheda
     */
    Board() {};

    /**
     * Costruttore parametrizzato scheda
     * @param id        int     identificativo
     * @param coord     Point2d coordinate schedina
     */
    Board(int id, Point2d coord) : id(id), coord(coord) {};

    /**
     * Costruttore parametrizzato scheda
     * @param id        int     identificativo
     * @param x         double  coordinata x schedina
     * @param y         double  coordinata y schedina
     * @param A         int     dBm a un metro schedina
     */
    Board(int id, double x, double y, int A) : id(id), A(A) {
        coord = Point2d{x, y};
    };

    /**
     * Getter ID
     * @return int      ID schedina
     */
    int getId() const;

    /**
     * Getter Coordinate schedina
     * @return  Point2d coordinate
     */
    const Point2d &getCoord() const;

    /**
     * Getter parametro di potenza a un metro
     * @return int  dBm a un metro
     */
    int getA() const;
};

#endif //ROOMMONITOR_BOARD_H
