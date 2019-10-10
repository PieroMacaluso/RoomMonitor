
#include "Ricevi.h"
#include <mutex>
#include <condition_variable>
#include <string>
#include <thread>
#include <iostream>
#include <deque>
#include <algorithm>
#include <atomic>
#include <vector>
void Ricevi::produci(string mac, int id,/* c'è ne bisogno???*/ string  hash, int rssi, string timestamp,/* tipo time o string??? */ string ssid, string board)
{
    lock_guard<mutex> lg(m);
    datiArrivati.mac=mac;
    datiArrivati.id = id;
    datiArrivati.hash = mac;
    datiArrivati.rssi = rssi;
    datiArrivati.timestamp = timestamp;
    datiArrivati.ssid = ssid;
    datiArrivati.board= board;
    ins.push_back(datiArrivati);
    //CHIAMO AGGREGA con pop.front
    cvNessunDato.notify_all();

}

