//
// Created by pieromack on 08/08/19.
//

#ifndef ROOMMONITOR_POSITIONDATA_H
#define ROOMMONITOR_POSITIONDATA_H


#include <iostream>
#include <sstream>

class PositionData {
    int n_packets;
    double x;
    double y;
public:
    PositionData(double x, double y) : x(x), y(y), n_packets(1) {}

    PositionData() : x(0), y(0), n_packets(0) {};

    void addPacket(double new_x, double new_y);

    void addPacketWeighted(double new_x, double new_y, double rsum);

    void addPacket(PositionData new_pos);

    void addPacketWeighted(PositionData new_pos, double rsum);


    friend std::ostream &operator<<(std::ostream &os, const PositionData &a);


    double getX() const {
        return x;
    }

    double getY() const {
        return y;
    }

    std::string getStringPosition() {
        std::ostringstream strX;
        std::ostringstream strY;
        strX << x;
        strY << y;
        return strX.str() + "," + strY.str();
    }
};


#endif //ROOMMONITOR_POSITIONDATA_H
