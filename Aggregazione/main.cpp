
#include "Aggregazione.h"
#include <iostream>
#include <thread>
#include <deque>

int main() {
    Aggregazione aggrega;
    int n;
    deque <dati> deq;
    //std::cout << "Hello World!\n";
    //std::thread first(&aggrega);
    std::thread first(&Aggregazione::leggi, &aggrega,  std::ref(n), std::ref(deq));
    first.join();
}