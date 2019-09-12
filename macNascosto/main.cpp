#include <iostream>
#include <deque>
#include <vector>
#include "Entry.h"

std::deque<Entry> allPackets;
std::deque<Entry> hiddenPackets;


void setHiddenPackets(){
    for(int i=0;i<allPackets.size();i++){
        if((allPackets.at(i).getMac().at(0)<='7' && allPackets.at(i).getMac().at(0)>='4') || allPackets.at(i).getMac().at(0)>='c'){
            std::cout << allPackets.at(i).getMac() << " hidden" <<std::endl;
            allPackets.at(i).setHidden(true);
        }else{
            std::cout << allPackets.at(i).getMac() << " not hidden" <<std::endl;
            allPackets.at(i).setHidden(false);
        }
    }
}


std::deque<Entry> getHiddenPackets(uint32_t initTime,uint32_t endTime){

    for(Entry e:allPackets)
        if(e.getHidden() && (e.getTimestamp()>=initTime || e.getTimestamp()<=endTime))
            hiddenPackets.push_back(e);

    return hiddenPackets;
}

/**
 * veririca se sono presenti mac simili a source oppure no
 * @param source
 * @param initTime
 * @param endTime
 * @return
 */
bool getHiddenDeviceFor(Entry source,uint32_t initTime,uint32_t endTime){
    //entro 5 minuti, stessa posizione +-0.5, altro da vedere
    uint32_t tolleranzaTimestamp=240;//usata per definire entro quanto la posizione deve essere uguale, 240= 4 minuti
    double tolleranzaX=0.5;
    double tolleranzaY=0.5;
    double perc;
    bool trovato= false;

    for(int j=0;j<hiddenPackets.size();j++){
        if(hiddenPackets.at(j).getMac()!=source.getMac()){
            double diff=(source.getTimestamp()<hiddenPackets.at(j).getTimestamp()) ? (hiddenPackets.at(j).getTimestamp()-source.getTimestamp()) : (source.getTimestamp()-hiddenPackets.at(j).getTimestamp());
            if(diff<=tolleranzaTimestamp){
                //mac diverso ad intervallo inferiore di 1 minuto
                double diffX=(source.getX()<hiddenPackets.at(j).getX()) ? (hiddenPackets.at(j).getX()-source.getX()) : (source.getX()-hiddenPackets.at(j).getX());
                double diffY=(source.getY()<hiddenPackets.at(j).getY()) ? (hiddenPackets.at(j).getY()-source.getY()) : (source.getY()-hiddenPackets.at(j).getY());
                if(diffX<=tolleranzaX && diffY<=tolleranzaY){
                    //mac diverso con posizione simile in 4 minuto=> possibile dire che sia lo stesso dispositivo
                    perc=((diffX*100/tolleranzaX) + (diffY*100/tolleranzaY) + (diff*100/tolleranzaTimestamp))*100/(300);
                    std::cout << source.getMac() << " simile ad " << hiddenPackets.at(j).getMac() << " con probabilita' del "<<perc<<"%" << std::endl;
                    trovato= true;
                }
            }

        }
    }

    return trovato;
}

/**
 * Restituisce una stima del numero di dispositivi con mac nascosto nell'intervallo di tempo passato.
 * Separato su due funzioni per avere anche solo la ricerca per un singolo mac.
 * @param initTime
 * @param endTime
 *
 * initTime e endTime corrispondono al periodo di osservazione (esempio alcune ore)
 * tolleranzaTimestamp corrisponde al tempo in cui la posizione deve essere simile per dire che il mac è uguale ad un altro (esempio 1 minuto)
 *
 * @return
 */

int getHiddenDevice(uint32_t initTime,uint32_t endTime){
    bool trovato;
    int numHiddenDevice=0;

    for(int i=0;i<hiddenPackets.size();i++){
        trovato=getHiddenDeviceFor(hiddenPackets.at(i),initTime,endTime);
        if(trovato)
            numHiddenDevice++;
    }

    return numHiddenDevice;
}

int main() {
    /*Simulazione popolamento db*/
    int numHiddenDevice;

    //std::deque<Entry> allPackets;

    std::string m("98:22:ef:ad:70:c9");
    Entry p(m, 1566306651,0.0,1.1); //1001 1000 VERO
    allPackets.push_back(p);

    std::string m1("7a:dd:69:d9:19:85");
    Entry p1(m1, 1566810856,1.0,1.2); //0111 1010 HIDDEN_1
    allPackets.push_back(p1);

    std::string m2("a0:4e:a7:99:37:3e");
    Entry p2(m2, 1566406713,5.0,2.0); //1010 0000 VERO
    allPackets.push_back(p2);

    std::string m3("de:0c:ab:b8:b2:6f");
    Entry p3(m3, 1566810729, 1.4, 0.9); //1101 1110 hidden_1
    allPackets.push_back(p3);

    std::string m4("ca:94:2c:13:4e:46");
    Entry p4(m4, 1566810654, 5.4, 7.9);  //1100 1010 hidden_2
    allPackets.push_back(p4);

    std::string m5("fe:8a:b9:d9:2a:c8");
    Entry p5(m5, 1566810858, 0.9, 1.5);  //1111 1110 hidden_1
    allPackets.push_back(p5);

    std::string m6("fe:ee:b9:d9:2a:c8");
    Entry p6(m6, 1566810668, 5.7, 7.5);  //1111 1110 hidden_2
    allPackets.push_back(p6);

    std::string m7("fe:ab:b9:d9:2a:c8");
    Entry p7(m7, 1566820858, 0.9, 1.5);  //1111 1110 hidden_3
    allPackets.push_back(p7);

    std::string m8("fe:bc:b9:d9:2a:c8");
    Entry p8(m8, 1566820850, 1.1, 1.5);  //1111 1110 hidden_3
    allPackets.push_back(p8);

    setHiddenPackets();       //todo inglobarlo nel calcolo della posizione in modo da avere sul db già un flag che indica se nascosto o meno
    hiddenPackets=getHiddenPackets(/*allPackets,*/0,1666810678);
    std::cout << "\nTrovati " << hiddenPackets.size() << " mac nascosti"<< std::endl;

    numHiddenDevice=getHiddenDevice(/*allPackets,*/0,1666810678);
    std::cout << "Numero dispositivi con mac nascosto stimato: "<<numHiddenDevice<<std::endl;

    std::cout<< "\nRicerca mac simili a " <<p3.getMac()<<"..."<< std::endl;
    getHiddenDeviceFor(p3,0,1666810678);

    return 0;
}