#include <iostream>
#include <deque>
#include <vector>
#include "Entry.h"


void setHiddenPackets(std::deque<Entry>& allPackets){
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


std::deque<Entry> getHiddenPackets(std::deque<Entry>& allPackets,uint32_t initTime,uint32_t endTime){
    std::deque<Entry> hiddenPackets;

    for(Entry e:allPackets)
        if(e.getHidden() && (e.getTimestamp()>=initTime || e.getTimestamp()<=endTime))
            hiddenPackets.push_back(e);

    return hiddenPackets;
}

/**
 * Restituisce una stima del numero di dispositivi con mac nascosto nell'intervallo di tempo passato
 * @param initTime
 * @param endTime
 *
 * initTime e endTime corrispondono al periodo di osservazione (esempio alcune ore)
 * tolleranzaTimestamp corrisponde al tempo in cui la posizione deve essere simile per dire che il mac è uguale ad un altro (esempio 1 minuto)
 *
 * @return
 */
 //DEPRECATA perchè con più di due paccheti nella stessa posizione non restituisce una stima valida.
 // Avendo i pacchetti A B e C inviati dallo stesso dispositivo con mac diversi, quando il primo for punta al pacchetto A funziona correttamente, ma quando passa a B conta un nuovo dispositivo vedendo C non riuscendo a verificare che è gia stato contato con A
 //todo possibile idea, eliminare tutti i pacchetti simili ad ogni giro, Così facendo quando il primo for punta ad A elimina B e C e non si dovrebbero avere problemi

/*int getHiddenDevice(std::deque<Entry>& allPackets,uint32_t initTime,uint32_t endTime){
    //entro 5 minuti, stessa posizione +-0.5, altro da vedere
    uint32_t tolleranzaTimestamp=240;//usata per definire entro quanto la posizione deve essere uguale, 240= 4 minuti
    double tolleranzaX=0.5;
    double tolleranzaY=0.5;
    bool trovato= false;

    int numHiddenDevice=0;

    std::deque<Entry> hiddenPackets=getHiddenPackets(allPackets,initTime,endTime);  //todo realizzare tramite query sul db
    std::cout << "\nTrovati " << hiddenPackets.size() << " mac nascosti"<< std::endl;


    for(int i=0;i<hiddenPackets.size();i++){
        trovato= false;
        for(int j=i+1;j<hiddenPackets.size();j++){
            if(hiddenPackets.at(j).getMac()!=hiddenPackets.at(i).getMac()){
                uint32_t diff=(hiddenPackets.at(i).getTimestamp()<hiddenPackets.at(j).getTimestamp()) ? (hiddenPackets.at(j).getTimestamp()-hiddenPackets.at(i).getTimestamp()) : (hiddenPackets.at(i).getTimestamp()-hiddenPackets.at(j).getTimestamp());
                if(diff<=tolleranzaTimestamp){
                    //mac diverso ad intervallo inferiore di 1 minuto
                    double diffX=(hiddenPackets.at(i).getX()<hiddenPackets.at(j).getX()) ? (hiddenPackets.at(j).getX()-hiddenPackets.at(i).getX()) : (hiddenPackets.at(i).getX()-hiddenPackets.at(j).getX());
                    double diffY=(hiddenPackets.at(i).getY()<hiddenPackets.at(j).getY()) ? (hiddenPackets.at(j).getY()-hiddenPackets.at(i).getY()) : (hiddenPackets.at(i).getY()-hiddenPackets.at(j).getY());
                    if(diffX<=tolleranzaX && diffY<=tolleranzaY){
                        //mac diverso con posizione simile in 1 minuto=> possibile dire che sia lo stesso dispositivo
                        std::cout << hiddenPackets.at(i).getMac() << " simile ad " << hiddenPackets.at(j).getMac() << std::endl;
                        //mac i associato al mac j, basta tornare al ciclo i, aumentare il contatore ed andare avanti. così riusciamo a contare il numero di gruppi di mac associati
                        trovato= true;
                        break;
                        //todo forse fare il secondo giro fino alla fine per raggruppare tutti quelli simili in una lista in modo da eliminarli per evitare di contarli più volte
                    }
                }

            }
        }
        if(trovato)
            numHiddenDevice++;
    }


    return numHiddenDevice;
}*/

/**
 * Nuova funzione per la stima del nunmero di dispositivi con mac nascosti.
 * Idea: mappare l'aula come tanti quadretti di dimensione pari alla tolleranza accettabile per dire che i vari pacchetti siano dello stesso dispositivo.
 *       Creare una mappa con chiave il numero del quadretto e con valore la lista di tutti i pacchetti, con mac non univoco e con posizione uguale a quella del quadretto corrispondente, nel lasso di tempo specificato.
 *       Data la posizione di un pacchetto in coordinate x e y si ricava il suo indice facendo x*(numero quadretti su x)+y.
 *       La stima è data dal conteggio di tutte le entry della mappa con lista pacchetti piena.
 * @return
 */
int getHiddenDevice(std::deque<Entry>& allPackets,uint32_t initTime,uint32_t endTime){
    //Così facendo abbiamo il problema di non rilvare un eventuale nuovo dispositivo nella stessa posizione se usiamo dei initTime e endTime troppo larghi. Nella funzione precedente si poteva verificare se il timestamp di due pacchetti fosse simile oltre a vedere la posizione, usando invece la mappa con le dimensioni non è possibile separare per timestamp diversi.
}
int main() {
    /*Simulazione popolamento db*/
    int numHiddenDevice;

    std::deque<Entry> allPackets;

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
    Entry p5(m5, 1566810678, 5.9, 7.6);  //1111 1110 hidden_2
    allPackets.push_back(p5);

    std::string m6("fe:ee:b9:d9:2a:c8");
    Entry p6(m6, 1566810668, 5.7, 7.5);  //1111 1110 hidden_2
    allPackets.push_back(p6);

    setHiddenPackets(allPackets);       //forse possibile inglobarlo nel calcolo della posizione in modo da avere sul db già un flag che indica se nascosto o meno

    numHiddenDevice=getHiddenDevice(allPackets,0,1666810678);
    std::cout << "Numero dispositivi con mac nascosto stimato: "<<numHiddenDevice<<std::endl;


    return 0;
}