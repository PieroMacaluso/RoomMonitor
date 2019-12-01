#include "PositionData.h"

void PositionData::addPacket(double new_x, double new_y) {
    // Running Average (media mobile) utilizzata per mediare le posizioni.
    this->x = this->x + (new_x - this->x) / (this->n_packets + 1);
    this->y = this->y + (new_y - this->y) / (this->n_packets + 1);
    this->n_packets++;

}

std::ostream &operator<<(std::ostream &os, const PositionData &a) {
    os << "Position( " << a.x << ", " << a.y << " )";
    return os;
}

PositionData PositionData::positionDataNull() {
    return PositionData(-100, -100);
}
