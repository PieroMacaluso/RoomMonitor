//
// Created by angel on 26/08/2019.
//

#ifndef MACNASCOSTO_ENTRY_H
#define MACNASCOSTO_ENTRY_H

#include <string>

class Entry {
    std::string macPeer;
    uint32_t timestamp;
    bool hidden;
    double x;
    double y;
public:
    Entry( std::string &mac, uint32_t timestamp, double x,double y);
    void setHidden(bool val);
    std::string getMac();
    bool getHidden();
    uint32_t getTimestamp();
    double getX();
    double getY();
};


#endif //MACNASCOSTO_ENTRY_H
