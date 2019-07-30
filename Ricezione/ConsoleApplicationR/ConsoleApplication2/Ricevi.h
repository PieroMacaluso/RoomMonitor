#pragma once
#include <mutex>
#include <condition_variable>
#include <string>
#include <thread>
#include <iostream>
#include <deque> 
#include <algorithm>
#include <atomic>
#include <vector>
using namespace std;

typedef struct campi
{
	string mac;
	int id;// c'è ne bisogno???
	string  hash;
	int rssi;
	string timestamp;// tipo time o string???
	string ssid;
	string board;
} dati;

class Ricevi
{
private:
	mutex m;
	condition_variable cvNessunDato;
	dati datiArrivati;
	deque<dati> ins;
	
	

	//deque <dati> deq;// all'inizio ne uso una sola magari nella ricezione 2 come avevamo detto e questa messa come parametro

public:
	Ricevi() {};
	~Ricevi() {};
	void produci(string mac, int id,/* c'è ne bisogno???*/ string  hash, int rssi, string timestamp,/* tipo time o string??? */ string ssid, string board);


};

