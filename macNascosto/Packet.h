//
// Created by angel on 26/08/2019.
//

#ifndef MACNASCOSTO_PACKET_H
#define MACNASCOSTO_PACKET_H

#include <string>

class Packet {
    std::string macPeer;
    int id_schedina;
    int rssi;
    std::string fcs;
    uint32_t timestamp;
    std::string ssid;
    double x;
    double y;
public:
    Packet(int id_schedina, std::string fcs, int rssi, std::string &mac, uint32_t timestamp, std::string &ssid, double x,
           double y);
};


#endif //MACNASCOSTO_PACKET_H
