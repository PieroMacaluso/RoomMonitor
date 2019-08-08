//
// Created by pieromack on 08/08/19.
//

#include "PositionData.h"

void PositionData::addPacket(float new_x, float new_y) {
    // Running Average (media mobile) utilizzata per mediare le posizioni.
    this->x = this->x + (new_x - this->x) / (this->n_packets + 1);
    this->y = this->y + (new_y - this->y) / (this->n_packets + 1);
    this->n_packets++;

}

void PositionData::addPacket(PositionData new_pos) {
    // Running Average (media mobile) utilizzata per mediare le posizioni.
    this->x = this->x + (new_pos.x - this->x) / (this->n_packets + 1);
    this->y = this->y + (new_pos.y - this->y) / (this->n_packets + 1);
    this->n_packets++;
}

std::ostream &operator<<(std::ostream &os, const PositionData &a) {
    os << "Position( " << a.x << ", " << a.y << " )";
    return os;
}
