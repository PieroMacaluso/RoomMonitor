#include <iostream>
#include <vector>
#include <math.h>
#include <deque>

using namespace std;

class Packet {
    int id_schedina;
    float x_schedina;
    float y_schedina;
    int rssi;
};

// TODO: Queste sono le specifiche di input e output
// PositionData fromRssiToXY(std::deque<Packet> deque);

/**
 * INPUT: std::deque<Packet> hash_group;
 * OUTPUT: PositionData del dispositivo
 * @return
 */
//calcolo distanza dispositivo rispetto ad ogni schedina. Alla fine aggrego rispetto a tutti i dispositivi
int main() {

    // hash_group.size() == numero delle schedine. La deque è riferita ai dati di un dispositivo.
    std::deque<Packet> hash_group;
    // TODO: Modifica strutture dati in base a quelle presenti in RoomMonitor
    typedef struct {
        float x;
        float y;
    } Coord;
    int n;// numero schedine

    float cost = 2;// n: Costante di propagazione del segnale. Costante 2 in ambiente aperto. TODO: vedere se applicabile a stanza
    float A = 8; // A: potenza del segnale ricevuto in dBm ad un metro TODO: da ricercare sperimentalmente
    float dist;
    float RSSI = 2; // RSSI: potenza del segnale ricevuto in dBm dalla schedina



    float x;//coordinate/soluzione del dispositivo
    float y1;
    float y2;
    float bSec; // coef b risoluzione equaz 2 grado
    float costa;// cost numerica derivante da calcoli
    cout << "inserisci il numero delle schedine" << endl;
    cin >> n;
    Coord daIns;
    vector<Coord> coord;// 2 vettori paralleli che contengono coordinate e distanza dispositivo da quella schedina
    vector<float> distanze;// Non ho usato mappa nel caso dobbiamo farlo concorrente
    for (int i = 0; i < n; i++) {
        cout << "inserisci la coordinata x della " << i << "° schedina " << endl;
        cin >> daIns.x;
        cout << "inserisci la coordinata y della " << i << "° schedina " << endl;
        cin >> daIns.y;
        coord.push_back(daIns);
        // double pow(double base, double altezza)
        dist = pow(10, (A - RSSI) / (10 * cost));
        distanze.push_back(dist);
        //cout<< "dist "<<  distanze[distanze.size()-1]<< endl;
    }

    /** ARRIVATI A QUESTO PUNTO ABBIAMO LE DISTANZE */

    // y1, y2 sono le soluzioni che nella tesi sono ym.

    // TODO: check se 2/3/4... circonferenze si intersecano altrimenti incrementarne il raggio di una quantità
    // Se la somma dei raggi di due circonferenze è maggiore rispetto alla distanza dei loro centri.

    //calcolo solo rispetto a 2 schedine poi FARE MEDIA???se più schedine(3° vettore parallelo di struct con con coordinate calcolate x e y rispetto a quella schedina)
    //ra                            rb
//    bSec=(pow(distanze[distanze.size()-1],2)-pow(distanze[distanze.size()-2],2)-pow(coord[distanze.size()-1].x,2)+pow(coord[distanze.size()-2].x,2)-pow(coord[distanze.size()-1].y, 2)+pow(coord[distanze.size()-2].y,2))/(-2*coord[distanze.size()-1].x+2*coord[distanze.size()-2].x)-2*coord[distanze.size()-2].x-2*coord[distanze.size()-2].y;

    costa = (pow(distanze[distanze.size() - 1], 2) - pow(distanze[distanze.size() - 2], 2) -
             pow(coord[distanze.size() - 1].x, 2) + pow(coord[distanze.size() - 2].x, 2) -
             pow(coord[distanze.size() - 1].y, 2) + pow(coord[distanze.size() - 2].y, 2)) /
            (-2 * coord[distanze.size() - 1].x + 2 * coord[distanze.size() - 2].x);
    cout << costa;
    // y1=(-bSec+sqrt(pow(bSec,2)-8*pow(coord[distanze.size()-2].y,2)-8*pow(distanze[distanze.size()-2],2)))/4;
    bSec = (coord[distanze.size() - 2].x * (2 * coord[distanze.size() - 1].y - 2 * coord[distanze.size() - 2].y) /
            (-2 * coord[distanze.size() - 1].x + 2 * coord[distanze.size() - 2].x) +
            coord[distanze.size() - 2].y);// applico formula equaz second grado ridotta
    float a;
    a = ((4 * coord[distanze.size() - 1].y + 4 * coord[distanze.size() - 2].y -
          8 * coord[distanze.size() - 1].y * coord[distanze.size() - 2].y) /
         pow(-2 * coord[distanze.size() - 1].x + 2 * coord[distanze.size() - 2].x, 2) + 1);
    y1 = (-bSec + sqrt(pow(bSec, 2) - a * (-2 * coord[distanze.size() - 2].x * costa + pow(costa, 2) +
                                           pow(coord[distanze.size() - 2].x, 2) + pow(coord[distanze.size() - 2].y, 2) -
                                           pow(distanze[distanze.size() - 2], 2)))) / a;

    cout << y1 << endl;
    //y2=(-bSec-sqrt(pow(bSec,2)-8*pow(coord[distanze.size()-2].y,2)-8*pow(distanze[distanze.size()-2],2)-8*(coord[distanze.size()-2].x,2)))/4;
    y2 = (-bSec - sqrt(pow(bSec, 2) - a * (-2 * coord[distanze.size() - 2].x * costa + pow(costa, 2) +
                                           pow(coord[distanze.size() - 2].x, 2) + pow(coord[distanze.size() - 2].y, 2) -
                                           pow(distanze[distanze.size() - 2], 2)))) / a;

    cout << y2 << endl;

    /** y1, y2**/
    /**
     * 1) y1, y2 coincidono -> Un punto -> RISULTATO (non accadrà MAI)
     * 2) y1, y2 sol distinte -> Due punti da calcolare x(1,2) e y(1,2) -> MEDIA ->  RISULTATO
     * 3) y1, y2 no soluzione -> Aumentare il raggio di 0,5/1m in maniera costante e ricalcolare y1, y2 riconducendoci al caso 1 o 2.
     *
     */

    /**
     *
     */
    //if controllo quale y minore di 0 e uso solo quella
    if (y1 < 0)
        if (y2 < 0)
            cout << "No soluzione" << endl;
        else {
            x = bSec + ((2 * coord[distanze.size() - 1].y * y2 - 2 * coord[distanze.size() - 2].y * y2) /
                        (-2 * coord[distanze.size() - 1].x + 2 * coord[distanze.size() - 2].x)) +
                2 * coord[distanze.size() - 2].x + 2 * coord[distanze.size() - 2].y;
            cout << "x: " << x << endl;
        }

    else {
        x = bSec + ((2 * coord[distanze.size() - 1].y * y1 - 2 * coord[distanze.size() - 2].y * y1) /
                    (-2 * coord[distanze.size() - 1].x + 2 * coord[distanze.size() - 2].x)) +
            2 * coord[distanze.size() - 2].x + 2 * coord[distanze.size() - 2].y;

        cout << "x: " << x << endl;
    }
}