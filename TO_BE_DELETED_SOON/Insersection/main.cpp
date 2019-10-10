/*
 * File:   main.cpp
 * Author: SAMARAS
 *
 * 13/10/13
 */

#include <cstdlib>
#include <iostream>
#include <cmath>
#include <deque>
#include <algorithm>
#include "PositionData.h"

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

/*
 * Find the intersection point(s) of two circles,
 * when their centers and radiuses are given (2D).
 */

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
    double norm() const {
        return sqrt(X * X + Y * Y);
    }

    void setCoords(double x, double y) {
        X = x;
        Y = y;
    }

    // Print point
    friend std::ostream &operator<<(std::ostream &s, const Point2d &p) {
        s << p.x() << " " << p.y();
        return s;
    }

private:
    double X;
    double Y;
};

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
    Circle(double R, double X, double Y)
            : r(R), c(X, Y) {}

    Point2d getC() const { return c; }

    double getR() const { return r; }

    bool containsPoint(Point2d x) {
        // distance between center and point
        double d = Point2d(c.x() - x.x(),
                           c.y() - x.y()).norm();
        return d <= this->getR();

    }

    size_t intersect(const Circle &C2, Point2d &i1, Point2d &i2) {
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

    // Print circle
    friend std::ostream &operator<<(std::ostream &s, const Circle &C) {
        s << "Center: " << C.getC() << ", r = " << C.getR();
        return s;
    }

private:
    // radius
    double r;
    // center
    Point2d c;

};

int main(void) {
    // radius and center of circles
    Circle a(2, 0, 0);
    Circle b(5, 6, 0);
    Circle c(3, 0, 4);
    Circle d(5.5, 6, 4);

    // Deque di cerchi
    std::deque<Circle> circles{};
    // Deque che ci serve per media pesata
    std::deque<std::pair<PositionData, double>> pointW;
    // Push cerchi
    circles.push_back(a);
    circles.push_back(b);
    circles.push_back(c);
    circles.push_back(d);
    // Punto medio finale (no pesato)
    PositionData final{};

    // TODO: controllare che tutte le circonferenze si intersechino
    // Combinazioni  e controllo, se anche una non è soddisfatta allora ingrandisco tutte le circonferenze dello spazio che manca.

    // Doppio ciclo per combinazioni
    for (int i = 0; i < circles.size() - 1; i++) {
        for (int j = i + 1; j < circles.size(); j++) {
//            std::cout << circles[i] << "\n" << circles[j] << "\n";

            // Punti di intersezione
            Point2d intPoint1, intPoint2;
            // Calcolare intersezione
            // TODO: rivedere bene funzione intersect
            size_t i_points = circles[i].intersect(circles[j], intPoint1, intPoint2);

            // Controllo se questi punti sono contenuti nelle altre circonferenze.

            // Ipotesi verificata a meno che non si trovi una condizione falsa.
            bool ok1 = true;
            bool ok2 = true;
            for (int k = 0; k < circles.size(); k++) {
                // Non controllare su circonferenze oggetto di intersezione
                if (k == i || k == j) continue;

                if (!circles[k].containsPoint(intPoint1)) {
                    ok1 = false;
                }
                if (!circles[k].containsPoint(intPoint2)) {
                    ok2 = false;
                }
            }

            if (ok1) {
                // CALCOLO CON MEDIA
                PositionData mean_res{};
                mean_res.addPacket(intPoint1.x(), intPoint1.y());
                pointW.emplace_back(mean_res, circles[i].getR() + circles[j].getR());
                // CALCOLO CON MEDIA PESATA
                final.addPacket(intPoint1.x(), intPoint1.y());

            }
            if (ok2) {
                // CALCOLO CON MEDIA
                PositionData mean_res{};
                mean_res.addPacket(intPoint2.x(), intPoint2.y());
                pointW.emplace_back(mean_res, circles[i].getR() + circles[j].getR());

                // CALCOLO CON MEDIA PESATA
                final.addPacket(intPoint2.x(), intPoint2.y());

            }
        }
    }

    // CALCOLO FINALE MEDIA PESATA
    double num_x = 0;
    double num_y = 0;
    double den = 0;
    std::for_each(pointW.begin(), pointW.end(),
                  [&](std::pair<PositionData, double> pair) {
                      num_x += pair.first.getX() / pair.second;
                      num_y += pair.first.getY() / pair.second;
                      den += 1 / pair.second;
                  });

    PositionData result{};
    result.addPacket(num_x / den, num_y / den);

    // Stampa media
    std::cout << final << std::endl;
    // Stampa media pesata
    std::cout << result << std::endl;

    // TODO: valido se x e y risultanti sono nella stanza, altrimenti no!


    return 0;
}