#include "Point2d.h"

double Point2d::norm() const {
    return sqrt(X * X + Y * Y);
}

void Point2d::setCoords(double x, double y) {
    X = x;
    Y = y;
}

std::ostream &operator<<(std::ostream &s, const Point2d &p) {
    s << p.x() << " " << p.y();
    return s;
}

double Point2d::distance(Point2d p2) {
    return Point2d(this->x() - p2.x(),
                   this->y() - p2.y()).norm();
}
