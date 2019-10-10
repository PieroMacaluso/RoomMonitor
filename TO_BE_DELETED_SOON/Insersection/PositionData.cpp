//
// Created by pieromack on 08/08/19.
//

#include "PositionData.h"

void PositionData::addPacket(double new_x, double new_y) {
    // Running Average (media mobile) utilizzata per mediare le posizioni.
    this->x = this->x + (new_x - this->x) / (this->n_packets + 1);
    this->y = this->y + (new_y - this->y) / (this->n_packets + 1);
    this->n_packets++;

}

void PositionData::addPacketWeighted(double new_x, double new_y, double rsum) {
    // Running Average (media mobile) utilizzata per mediare le posizioni.
    this->x = this->x + ((new_x / rsum) - this->x) / (this->n_packets + 1 / rsum);
    this->y = this->y + ((new_y / rsum) - this->y) / (this->n_packets + 1 / rsum);
    this->n_packets += 1 / rsum;


}

void PositionData::addPacket(PositionData new_pos) {
    // Running Average (media mobile) utilizzata per mediare le posizioni.
    this->x = this->x + (new_pos.x - this->x) / (this->n_packets + 1);
    this->y = this->y + (new_pos.y - this->y) / (this->n_packets + 1);
    this->n_packets++;
}

void PositionData::addPacketWeighted(PositionData new_pos, double rsum) {
    // Running Average (media mobile) utilizzata per mediare le posizioni.
    this->x = this->x + (new_pos.x / rsum - this->x) / (this->n_packets + 1 / rsum);
    this->y = this->y + (new_pos.y / rsum - this->y) / (this->n_packets + 1 / rsum);
    this->n_packets += 1 / rsum;
}

std::ostream &operator<<(std::ostream &os, const PositionData &a) {
    os << "Position( " << a.x << ", " << a.y << " )";
    return os;
}
