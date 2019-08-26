//
// Created by angel on 26/08/2019.
//

#include "Packet.h"

Packet::Packet(int id_schedina, std::string fcs, int rssi, std::string &mac, uint32_t timestamp, std::string &ssid,
               double x, double y) :
        id_schedina(id_schedina),macPeer(mac),ssid(ssid),fcs(fcs),timestamp(timestamp),rssi(rssi), x(x),y(y) {}