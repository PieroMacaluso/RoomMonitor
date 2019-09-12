//
// Created by pieromack on 11/08/19.
//

#ifndef ROOMMONITOR_CIRCLE_H
#define ROOMMONITOR_CIRCLE_H

#include "Point2d.h"
#include <cmath>

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

class Circle {
public:
    /**
     * @param R - radius
     * @param C - center
     */
    Circle(double R, Point2d &C)
            : r(R), c(C) {}

    /**
     * @param R - radius
     * @param X - center's x coordinate
     * @param Y - center's y coordinate
     */
    Circle(double R, double X, double Y);

    Point2d getC() const;

    double getR() const;

    void increaseR(double increment);

    bool containsPoint(Point2d x);

    size_t intersect(const Circle &C2, Point2d &i1, Point2d &i2);

    // Print circle
    friend std::ostream &operator<<(std::ostream &s, const Circle &C);

private:
    // radius
    double r;
    // center
    Point2d c;

};


#endif //ROOMMONITOR_CIRCLE_H
