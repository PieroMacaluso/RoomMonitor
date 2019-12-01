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
     * Costruttore cerchio paraemtrizzato
     * @param R - radius
     * @param C - center
     */
    Circle(double R, Point2d &C)
            : r(R), c(C) {}

    /**
     * Costruttore cerchio paraemtrizzato
     * @param R - radius
     * @param X - center's x coordinate
     * @param Y - center's y coordinate
     */
    Circle(double R, double X, double Y);

    /**
     * Getter coordinate
     * @return Point2d      Coordinate
     */
    Point2d getC() const;

    /**
     * Getter Raggio
     * @return double       Raggio
     */
    double getR() const;

    /**
     * Incremento (o decremento se negativo) raggio di un double
     * @param increment
     */
    void increaseR(double increment);

    /**
     * Check if the Circle contains a specific point
     * @param x Point2d     Point to be checked
     * @return true if it is contained, false otherwise
     */
    bool containsPoint(Point2d x);

    /**
     * Check if the circle object intersects the circle specified as parameter
     * @param C2 Circle     Circle to check
     * @param i1 Point2d    Reference first intersection
     * @param i2 Point2d    Reference second intersection
     * @return  0           circles too far apart
     *          1           one intersection
     *          2           two intersections
     *          -1          Circle coincides
     *          -2          One circle contains the other, but they do not coincide
     */
    int intersect(const Circle &C2, Point2d &i1, Point2d &i2);

    /**
     * Operatore << per eseguire la print del cerchio, usato solo in debug
     * @param s std::stream     stream
     * @param C Circle          cerchio
     * @return  std::ostream    stream output
     */
    friend std::ostream &operator<<(std::ostream &s, const Circle &C);

private:
    /**
     * Raggio
     */
    double r;
    /**
     * Centro
     */
    Point2d c;

};


#endif //ROOMMONITOR_CIRCLE_H
