#include <iostream>
#include <vector>
#include <math.h>
using namespace std;
//calcolo distanza dispositivo rispetto ad ogni schedina. Alla fine aggrego rispetto a tutti i dispositivi
int main() {
    typedef struct {
        float x;
        float y;
    } Coord;
    int n;// numero schedine

    float cost=2;//Costante di propagazione del segnale. Costante 2 in ambiente aperto.
    float A=8; // potenza del segnale ricevuto in dBm: 2 solo x test
    float dist;
    float RSSI=2;// 2 solo per test



    float x;//coordinate/soluzione del dispositivo
    float y1;
    float y2;
    float bSec; //coef b risoluzione equaz 2 grado
    float costa;// cost numerica derivante da calcoli
    cout << "inserisci il numero delle schedine" << endl;
    cin>>n;
    Coord daIns;
    vector<Coord> coord;// 2 vettori paralleli che contengono coordinate e distanza dispositivo da quella schedina
    vector<float> distanze;// Non ho usato mappa nel caso dobbiamo farlo concorrente
    for(int i=0;i<n;i++)
    {
        cout << "inserisci la coordinata x della "<< i <<"° schedina " << endl;
        cin>>daIns.x;
        cout << "inserisci la coordinata y della "<< i <<"° schedina " << endl;
        cin>>daIns.y;
        coord.push_back(daIns);
       // double pow(double base, double altezza)
       dist= pow(10, (A-RSSI)/(10*cost) );
       distanze.push_back(dist);
       //cout<< "dist "<<  distanze[distanze.size()-1]<< endl;
    }
    //calcolo solo rispetto a 2 schedine poi FARE MEDIA???se più schedine(3° vettore parallelo di struct con con coordinate calcolate x e y rispetto a quella schedina)
    //ra                            rb
//    bSec=(pow(distanze[distanze.size()-1],2)-pow(distanze[distanze.size()-2],2)-pow(coord[distanze.size()-1].x,2)+pow(coord[distanze.size()-2].x,2)-pow(coord[distanze.size()-1].y, 2)+pow(coord[distanze.size()-2].y,2))/(-2*coord[distanze.size()-1].x+2*coord[distanze.size()-2].x)-2*coord[distanze.size()-2].x-2*coord[distanze.size()-2].y;

    costa=(pow(distanze[distanze.size()-1],2)-pow(distanze[distanze.size()-2],2)-pow(coord[distanze.size()-1].x,2)+pow(coord[distanze.size()-2].x,2)-pow(coord[distanze.size()-1].y, 2)+pow(coord[distanze.size()-2].y,2))/(-2*coord[distanze.size()-1].x+2*coord[distanze.size()-2].x);
cout<<costa;
   // y1=(-bSec+sqrt(pow(bSec,2)-8*pow(coord[distanze.size()-2].y,2)-8*pow(distanze[distanze.size()-2],2)))/4;
   bSec= (coord[distanze.size()-2].x*(2*coord[distanze.size()-1].y-2*coord[distanze.size()-2].y)/(-2*coord[distanze.size()-1].x+2*coord[distanze.size()-2].x)+coord[distanze.size()-2].y);// applico formula equaz second grado ridotta
   float a;
   a=((4*coord[distanze.size()-1].y+4*coord[distanze.size()-2].y-8*coord[distanze.size()-1].y*coord[distanze.size()-2].y)/pow(-2*coord[distanze.size()-1].x+2*coord[distanze.size()-2].x,2)+1);
   y1=(-bSec+sqrt(pow(bSec,2)-a*(-2*coord[distanze.size()-2].x*costa+pow(costa,2)+pow(coord[distanze.size()-2].x,2)+pow(coord[distanze.size()-2].y,2)-pow(distanze[distanze.size()-2],2))))/a;

    cout<<y1<<endl;
    //y2=(-bSec-sqrt(pow(bSec,2)-8*pow(coord[distanze.size()-2].y,2)-8*pow(distanze[distanze.size()-2],2)-8*(coord[distanze.size()-2].x,2)))/4;
    y2=(-bSec-sqrt(pow(bSec,2)-a*(-2*coord[distanze.size()-2].x*costa+pow(costa,2)+pow(coord[distanze.size()-2].x,2)+pow(coord[distanze.size()-2].y,2)-pow(distanze[distanze.size()-2],2))))/a;

    cout<<y2<<endl;
    //if controllo quale y minore di 0 e uso solo quella
    if(y1<0)
        if(y2<0)
            cout<<"No soluzione"<<endl;
        else
        {
            x=bSec+((2*coord[distanze.size()-1].y*y2-2*coord[distanze.size()-2].y*y2)/(-2*coord[distanze.size()-1].x+2*coord[distanze.size()-2].x))+2*coord[distanze.size()-2].x+2*coord[distanze.size()-2].y;
            cout<<"x: "<<x<< endl;
        }

    else{
        x = bSec + ((2 * coord[distanze.size()-1].y * y1 - 2 * coord[distanze.size() - 2].y * y1) /
                    (-2 * coord[distanze.size()-1].x + 2 * coord[distanze.size() - 2].x)) +
            2 * coord[distanze.size() - 2].x + 2 * coord[distanze.size() - 2].y;

        cout<<"x: "<<x<< endl;
    }
}