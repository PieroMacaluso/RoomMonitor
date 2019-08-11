//
// Created by pieromack on 11/08/19.
//

#ifndef ROOMMONITOR_POINT2D_H
#define ROOMMONITOR_POINT2D_H


#include <iostream>
#include <cmath>

class Point2d {
public:
    Point2d() {}

    Point2d(double x, double y)
            : X(x), Y(y) {}

    double x() const { return X; }

    double y() const { return Y; }

    /**
     * Returns the norm of this vector.
     * @return the norm
    */
    double norm() const;

    void setCoords(double x, double y);

    // Print point
    friend std::ostream &operator<<(std::ostream &s, const Point2d &p);

private:
    double X;
    double Y;
};


#endif //ROOMMONITOR_POINT2D_H
