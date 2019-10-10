#include <iostream>
#include "DequeThreadSafe.h"
#include "Packet.h"
#include "Ricezione.h"
int main()
{
    DequeThreadSafe<Packet> pacchettiRicevuti;
    //todo impostare interfaccia grafica
    Ricezione(std::make_shared<DequeThreadSafe<Packet>>(pacchettiRicevuti));

    return 0;
}
