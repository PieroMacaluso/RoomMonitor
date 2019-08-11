//
// Created by pieromack on 11/08/19.
//

#include "Circle.h"

Circle::Circle(double R, double X, double Y)
        : r(R), c(X, Y) {}

Point2d Circle::getC() const { return c; }

double Circle::getR() const { return r; }

bool Circle::containsPoint(Point2d x) {
    // distance between center and point
    double d = Point2d(c.x() - x.x(),
                       c.y() - x.y()).norm();
    return d <= this->getR();

}

size_t Circle::intersect(const Circle &C2, Point2d &i1, Point2d &i2) {
    // distance between the centers
    double d = Point2d(c.x() - C2.c.x(),
                       c.y() - C2.c.y()).norm();

    // find number of solutions
    if (d > r + C2.r) // circles are too far apart, no solution(s)
    {
        std::cout << "Circles are too far apart\n";
        return 0;
    } else if (d == 0 && r == C2.r) // circles coincide
    {
        std::cout << "Circles coincide\n";
        return 0;
    }
        // one circle contains the other
    else if (d + min(r, C2.r) < max(r, C2.r)) {
        std::cout << "One circle contains the other\n";
        return 0;
    } else {
        double a = (r * r - C2.r * C2.r + d * d) / (2.0 * d);
        double h = sqrt(r * r - a * a);

        // find p2
        Point2d p2(c.x() + (a * (C2.c.x() - c.x())) / d,
                   c.y() + (a * (C2.c.y() - c.y())) / d);

        // find intersection points p3
        i1.setCoords(p2.x() + (h * (C2.c.y() - c.y()) / d),
                     p2.y() - (h * (C2.c.x() - c.x()) / d)
        );
        i2.setCoords(p2.x() - (h * (C2.c.y() - c.y()) / d),
                     p2.y() + (h * (C2.c.x() - c.x()) / d)
        );

        if (d == r + C2.r)
            return 1;
        return 2;
    }
}

std::ostream &operator<<(std::ostream &s, const Circle &C) {
    s << "Center: " << C.getC() << ", r = " << C.getR();
    return s;
}
