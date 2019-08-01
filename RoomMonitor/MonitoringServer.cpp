//
// Created by pieromack on 26/07/19.
//

#include "MonitoringServer.h"

std::deque<Packet> MonitoringServer::string2packet(const std::vector<std::string>& p) {
    std::deque<Packet> deque;
    std::string ssid;

    for(const std::string& s:p){
        std::vector<std::string> values;
        split(s,values,',');

        if(values.size()==7)        //alcuni pacchetti contengono l'ssid e altri no
            //no ssid
            ssid="Nan";
        else
            ssid=values[7];

        Packet packet(std::stoi(values[0]),values[3],std::stoi(values[4]),values[5],std::stoi(values[6]),ssid);
        deque.push_back(packet);
    }

    return deque;
}

template<class Container>
void MonitoringServer::split(const std::string &str, Container &cont, char delim){
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim)) {
        cont.push_back(token);
    }
}
