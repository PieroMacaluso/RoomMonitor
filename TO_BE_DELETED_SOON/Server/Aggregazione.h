//
// Created by angel on 03/08/2019.
//

#ifndef SERVER_AGGREGAZIONE_H
#define SERVER_AGGREGAZIONE_H

#include "Packet.h"
#include "DequeThreadSafe.h"

class Aggregazione {
public:
    void leggi(int nSched, DequeThreadSafe<Packet> &deq/*Definire costruttore di copia/assegnazione o movimento???*/);

};


#endif //SERVER_AGGREGAZIONE_H
