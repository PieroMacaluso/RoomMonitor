//
// Created by angel on 31/07/2019.
//

#include <sstream>
#include "Packet.h"

Packet::Packet(int id_schedina, std::string fcs, int rssi, std::string &mac, uint32_t timestamp, std::string &ssid) :
        id_schedina(id_schedina),macPeer(mac),ssid(ssid),fcs(fcs),timestamp(timestamp),rssi(rssi) {}

std::ostream& operator<<(std::ostream& os, const Packet& pk)
{
    os << "Id:" << pk.id_schedina << " Fcs:" << pk.fcs << " Rssi:" << pk.rssi
       << " MacPeer:" << pk.macPeer << " Timestamp:"<< pk.timestamp << " Ssid:" << pk.ssid;
    return os;
}

const std::string &Packet::getMacPeer() const {
    return macPeer;
}

int Packet::getIdSchedina() const {
    return id_schedina;
}

int Packet::getRssi() const {
    return rssi;
}

const std::string &Packet::getFcs() const {
    return fcs;
}

uint32_t Packet::getTimestamp() const {
    return timestamp;
}

const std::string &Packet::getSsid() const {
    return ssid;
}


