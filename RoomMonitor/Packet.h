//
// Created by angel on 31/07/2019.
//

#ifndef ROOMMONITOR_PACKET_H
#define ROOMMONITOR_PACKET_H


#include <string>
#include <deque>
#include <vector>

class Packet {
    std::string macPeer;
    int id_schedina;
    int rssi;
    uint32_t fcs;
    uint32_t timestamp;
    std::string ssid;

public:
    Packet(int id_schedina, uint32_t fcs, int rssi, std::string &mac, uint32_t timestamp, std::string &ssid);
    friend std::ostream& operator<<(std::ostream& os, const Packet& pk);
};



#endif //ROOMMONITOR_PACKET_H
