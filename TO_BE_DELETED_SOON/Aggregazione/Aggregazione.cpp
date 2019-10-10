//

//
#include "Aggregazione.h"
#include <mutex>
#include <condition_variable>
#include <string>
#include <thread>
#include <iostream>


#include <deque>
#include <algorithm>
#include <atomic>
#include <vector>
/*Aggregazione::Aggregazione()
{
}
Aggregazione::~Aggregazione()
{
}*/
void Aggregazione::leggi(int nSched, deque <dati> &deq/*Definire costruttore di copia/assegnazione o movimento???*/)
{
    unique_lock<mutex> ul(m);
    //while(1) ciclo infinito???
    if (deq.size() == 0)
        cvNessunDato.wait(ul);// da fare notify proveniente da ricevi
    deque<dati>::iterator it = deq.begin();

    while (it != deq.end())// controllo per ogni mac che quel mac sia stato ricevuto da tutte le nsched
    {

        for (auto i = deq.crbegin(); i != deq.crend(); ++i)
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
        *it++;


    }
    deq.clear();
    // Sleep this thread for 1 min
    std::this_thread::sleep_for(std::chrono::milliseconds(60000));//METTERLO QUA???

}