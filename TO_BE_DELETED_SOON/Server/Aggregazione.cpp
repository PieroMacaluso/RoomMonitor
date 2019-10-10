//
// Created by angel on 03/08/2019.
//

#include <thread>
#include "Aggregazione.h"


void Aggregazione::leggi(int nSched,  DequeThreadSafe<Packet> &deq/*Definire costruttore di copia/assegnazione o movimento???*/)
{
    // controllo per ogni mac che quel mac sia stato ricevuto da tutte le nsched

    std::deque<Packet>::iterator it = deq.begin();


    while (it != deq.end())
    {
        //todo usare oggetto packet
        /*for (auto i = deq.crbegin(); i != deq.crend(); ++i)
        {
            if (i->mac == it->mac)
                if (find(ssid.begin(), ssid.end(), it->ssid) == ssid.end())
                    ssid.push_back(it->ssid);

        }
        if (ssid.size() >= (unsigned int) nSched)
        {
            //query INSERT
            ssid.clear();
        }
        *it++;*/


    }
    deq.clear();
    // Sleep this thread for 1 min
    std::this_thread::sleep_for(std::chrono::milliseconds(60000));//METTERLO QUA???

}