//
// Created by angel on 26/08/2019.
//

#include "Entry.h"

Entry::Entry(std::string &mac, uint32_t timestamp, double x, double y): macPeer(mac),timestamp(timestamp), x(x),y(y) {

}

void Entry::setHidden(bool val) {
    this->hidden=val;
}

std::string Entry::getMac() {
    return macPeer;
}

bool Entry::getHidden() {
    return hidden;
}

uint32_t Entry::getTimestamp() {
    return timestamp;
}

double Entry::getX() {
    return x;
}

double Entry::getY() {
    return y;
}
