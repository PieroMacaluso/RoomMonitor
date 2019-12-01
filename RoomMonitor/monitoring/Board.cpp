#include "Board.h"

int Board::getId() const {
    return id;
}

const Point2d &Board::getCoord() const {
    return coord;
}

int Board::getA() const {
    return A;
}
