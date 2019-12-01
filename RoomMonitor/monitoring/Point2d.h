#ifndef ROOMMONITOR_POINT2D_H
#define ROOMMONITOR_POINT2D_H


#include <iostream>
#include <cmath>

class Point2d {
public:
    /**
     * Costruttore vuoto
     */
    Point2d() {}

    /**
     * Costruttore parametrizzato
     * @param x     pos x
     * @param y     pos y
     */
    Point2d(double x, double y) : X(x), Y(y) {}

    /**
     * Getter x
     * @return pos x
     */
    double x() const { return X; }

    /**
     * Getter y
     * @return pos y
     */
    double y() const { return Y; }

    /**
     * Returns the norm of this vector.
     * @return the norm
    */
    double norm() const;

    /**
     * Distance between this point and p2
     * @param p2 Point 2
     * @return double distance
     */
    double distance(Point2d p2);

    /**
     *
     * Setter coordinates
     * @param x     pos x
     * @param y     pos y
     */
    void setCoords(double x, double y);

    /**
     * Operator << for debug printout
     * @param s     output stream
     * @param p     point to print
     * @return      outpust stream
     */
    friend std::ostream &operator<<(std::ostream &s, const Point2d &p);

private:
    /**
     * Coordinate X
     */
    double X;
    /**
     * Coordinate Y
     */
    double Y;
};


#endif //ROOMMONITOR_POINT2D_H
