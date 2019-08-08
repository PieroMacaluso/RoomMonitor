//
// Created by pieromack on 08/08/19.
//

#ifndef ROOMMONITOR_POSITIONDATA_H
#define ROOMMONITOR_POSITIONDATA_H


#include <iostream>

class PositionData {
    int n_packets;
    float x;
    float y;
public:
    PositionData(float x, float y): x(x), y(y), n_packets(1){};
    void addPacket(float new_x, float new_y);
    void addPacket(PositionData new_pos);
    friend std::ostream& operator<<(std::ostream& os, const PositionData &a);

};


#endif //ROOMMONITOR_POSITIONDATA_H
