//
// Created by pieromack on 11/08/19.
//

#ifndef ROOMMONITOR_BOARD_H
#define ROOMMONITOR_BOARD_H


#include "Point2d.h"
#include <QMetaType>

class Board {
    int id;
    Point2d coord;
    int A;
public:
    Board(){};
    Board(int id, Point2d coord) : id(id), coord(coord) {};

    Board(int id, double x, double y, int A) : id(id), A(A) {
        coord = Point2d{x, y};
    };


    int getId() const;

    const Point2d &getCoord() const;

    int getA() const;

    static std::map<int, Board> extract_from_setting();
};
Q_DECLARE_METATYPE(Board)


#endif //ROOMMONITOR_BOARD_H
