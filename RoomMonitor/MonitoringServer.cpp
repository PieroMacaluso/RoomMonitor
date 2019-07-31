//
// Created by pieromack on 26/07/19.
//

#include "MonitoringServer.h"

std::deque<Packet> MonitoringServer::string2packet(std::vector<std::string> p) {
    std::deque<Packet> packets;
    std::vector<std::string> values;
    std::string ssid;

    for(std::string s:p){
        split(s,values,' ');

        if(values.size()==7)        //alcuni pacchetti contengono l'ssid e altri no
            //no ssid
            ssid="Nan";
        else
            ssid=values[7];

        Packet p(std::stoi(values[0]),std::stoi(values[3]),std::stoi(values[4]),values[5],std::stoi(values[6]),ssid);
        packets.push_back(p);
    }

    return packets;
}

template<class Container>
void MonitoringServer::split(const std::string &str, Container &cont, char delim){
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim)) {
        cont.push_back(token);
    }
}
