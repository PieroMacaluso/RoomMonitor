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
class Aggregazione
{

private:
	mutex m;
	condition_variable cvNessunDato;
	atomic<int> count;
	vector<string> ssid;
	//deque <dati> deq;// all'inizio ne uso una sola magari nella ricezione 2 come avevamo detto e questa messa come parametro
public:
	Aggregazione():count(0) {};
	~Aggregazione();
	
	void leggi(int nSched, deque <dati> *deq/*Definire costruttore di copia/assegnazione o movimento???*/);
	
};

