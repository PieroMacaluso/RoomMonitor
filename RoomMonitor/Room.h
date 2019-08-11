//
// Created by pieromack on 11/08/19.
//

#include "Point2d.h"

#ifndef ROOMMONITOR_ROOM_H
#define ROOMMONITOR_ROOM_H


class Room {
    int id;
    Point2d max;
public:
    Room(int id, Point2d max) : id(id), max(max) {};

    Room(int id, double x, double y) : id(id) {
        max = Point2d{x, y};
    };


    int getId() const;

    const Point2d &getMax() const;
};


#endif //ROOMMONITOR_ROOM_H
