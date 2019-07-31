#include <iostream>
#include "Ricevi.h"
#include <iostream>
#include <thread>
#include <vector>
using namespace std;
int main() {
    Ricevi ric;


    vector<thread> produttori;
    int Nproduttori;

    string mac;
    int id;// c'è ne bisogno???
    string  hash;
    int rssi;
    string timestamp;// tipo time o string???
    string ssid;
    string board;

    cin >> Nproduttori ;
    for (int i = 0; i < Nproduttori; i++)
    {

        produttori.push_back(thread(

                [i, &ric, mac, id,
                        hash, rssi, timestamp,
                        ssid, board]()
                {
                    ric.produci(mac, id,
                                hash, rssi, timestamp,
                                ssid, board);
                }
                             )
        );

    }
    for (int i = 0; i < Nproduttori; i++) produttori[i].detach();//jon????


}