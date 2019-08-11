#include <iostream>
#include <vector>
#include <math.h>
#include <deque>
#include <cstdlib>
#include <cmath>

using namespace std;
//DA QUI NON MODIFICATO STESSO DEL FILE TROVATO
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

/*
 * Find the intersection point(s) of two circles,
 * when their centers and radiuses are given (2D).
 */

class Point2d{
public:
    Point2d() {}
    Point2d(double x, double y)
            : X(x), Y(y) {}

    double x() const { return X; }
    double y() const { return Y; }

    /**
     * Returns the norm of this vector.
     * @return the norm
    */
    double norm() const {
        return sqrt( X * X + Y * Y );
    }

    void setCoords(double x, double y) {
        X = x; Y = y;
    }

    // Print point
    friend std::ostream& operator << ( std::ostream& s, const Point2d& p )  {
        s << p.x() << " " << p.y();
        return s;
    }
private:
    double X;
    double Y;
};

class Circle{
public:
    /**
     * @param R - radius
     * @param C - center
     */
    Circle(double R, Point2d& C)
            : r(R), c(C) {}

    /**
     * @param R - radius
     * @param X - center's x coordinate
     * @param Y - center's y coordinate
     */
    Circle(double R, double X, double Y)
            : r(R), c(X, Y) {}

    Point2d getC() const { return c; }
    double getR() const { return r; }

    size_t intersect(const Circle& C2, Point2d& i1, Point2d& i2) {
        // distance between the centers
        double d = Point2d(c.x() - C2.c.x(),
                           c.y() - C2.c.y()).norm();

        // find number of solutions
        if(d > r + C2.r) // circles are too far apart, no solution(s)
        {
            std::cout << "Circles are too far apart\n";
            return 0;
        }
        else if(d == 0 && r == C2.r) // circles coincide
            {
            std::cout << "Circles coincide\n";
            return 0;
            }
            // one circle contains the other
            else if(d + min(r, C2.r) < max(r, C2.r))
                 {
            std::cout << "One circle contains the other\n";
            return 0;
                 }
                else
                    {
                        double a = (r*r - C2.r*C2.r + d*d)/ (2.0*d);
                        double h = sqrt(r*r - a*a);

                        // find p2
                        Point2d p2( c.x() + (a * (C2.c.x() - c.x())) / d,
                        c.y() + (a * (C2.c.y() - c.y())) / d);

                        // find intersection points p3
                        i1.setCoords( p2.x() + (h * (C2.c.y() - c.y())/ d),
                          p2.y() - (h * (C2.c.x() - c.x())/ d)
                                    );
                        i2.setCoords( p2.x() - (h * (C2.c.y() - c.y())/ d),
                          p2.y() + (h * (C2.c.x() - c.x())/ d)
                                     );

                        if(d == r + C2.r)
                         return 1;
                        return 2;
                    }
    }

    // Print circle
    friend std::ostream& operator << ( std::ostream& s, const Circle& C )  {
        s << "Center: " << C.getC() << ", r = " << C.getR();
        return s;
    }
private:
    // radius
    double r;
    // center
    Point2d c;

};
//FINO A QUI NO MODIFICHE
//ATTENZIONE A CONVERSIONI (DOUBLE CLASSI TROVATE) (FLOAT ROOMMONITOR)
class Packet {
public:
    int id_schedina;
    float x_schedina;
    float y_schedina;
    int rssi;
};

//
// PositionData fromRssiToXY(std::deque<Packet> deque);
//class PositionData {
//    int n_packets;
//    float x;
//    float y;
//public:
//    PositionData(float x, float y): x(x), y(y), n_packets(1){};
//    void addPacket(float new_x, float new_y);
//    void addPacket(PositionData new_pos);
//    friend std::ostream& operator<<(std::ostream& os, const PositionData &a);
//
//};
/**
 * INPUT: std::deque<Packet> hash_group;
 * OUTPUT: PositionData del dispositivo
 * @return
 */
/** y1, y2**/
/**
 * 1) y1, y2 coincidono -> Un punto -> RISULTATO (non accadrà MAI)
 * 2) y1, y2 sol distinte -> Due punti da calcolare x(1,2) e y(1,2) -> MEDIA ->  RISULTATO
 * 3) y1, y2 no soluzione -> Aumentare il raggio di 0,5/1m in maniera costante e ricalcolare y1, y2 riconducendoci al caso 1 o 2.

 */
//calcolo distanza dispositivo rispetto ad ogni schedina. Alla fine aggrego rispetto a tutti i dispositivi
int main() {

    // hash_group.size() == numero delle schedine. La deque è riferita ai dati di un dispositivo.
    std::deque<Packet> hash_group;



    float cost = 2;// n: Costante di propagazione del segnale. Costante 2 in ambiente aperto. TODO: vedere se applicabile a stanza
    float A = 8; // A: potenza del segnale ricevuto in dBm ad un metro TODO: da ricercare sperimentalmente
    float dist;
    float RSSI = 2; // RSSI: potenza del segnale ricevuto in dBm dalla schedina





    //AGGIUNGERE CAMPO A STRUCT ESISTENTI???
    vector<float> distanze;//  vettore parallelo (a coordinate schedina) che contiene distanza dispositivo da quella schedina

    struct coord{
        float x;
        float y;
    };

    vector<Point2d> ris;//salvo i risultati degli intersect e ne faccio la media per il ris finale opp salvo ris combinazioni
    vector<Circle> cerchi; // vettore di cerchi usato anche nel caso di necessità per trovare tutte le combinazioni
    Point2d i1, i2; //  punti intersezione i1(x,y) i2(x,y)
    //altro ciclo per fare le combinazioni???
    for (int i = 0; i < hash_group.size(); i++) {// per ogni schedina calcolo distanza dispositivo e quella schedina

        // double pow(double base, double altezza)
        dist = pow(10, (A - hash_group[i].rssi) / (10 * cost));
        distanze.push_back(dist);// alla fine non serve tenerlo memorizzato
        if(i<2)// per i primi 2 cerchi memorizzo poi calcolo intersezione rispetto al risultato a seguire(ris+nuovo->ris+nuovo...) opp IO L'HO FATTO COME CALCOLO IL RISULTATO E FACCIO MEDIA SU TUTTI I RISULTATI ALLA FINE (su vettore ris)
        {
            Circle c1(dist,  hash_group[i].x_schedina,  hash_group[i].y_schedina);//cerchio del dispositivo rispetto alla i schedina
            cerchi.push_back(c1);//alla fine penso non serva  tenerlo memorizzato
        }
        else{// salvare(es in vector<coord> ris) e farne la media per le coordinate finali più precise opp altra soluzione con combinazioni
            size_t i_points = cerchi[i-2].intersect(cerchi[i-1], i1, i2);
            //controllo quante intersezioni ci sono
            while (i_points<1)// nessuna intersezione devo aumentare il raggio fino a quando  c'è intersezione
            {
                dist=dist+0.5;// incremento il raggio di 0.5
                size_t i_points = cerchi[i-2].intersect(cerchi[i-1], i1, i2);
            }
            if(i_points == 2)
            {//le prime due posizioni non saranno utilizzate(divido per n-2 nella media) per avere parallelismo
                //assegnazione
               /* ris[0].x()=0;
                ris[1].y()=0;
                ris[i].x()=(i1.x()+i2.x())/2;//CORRETTO??
                ris[i].y()=(i1.y()+i2.y())/2*/
            }
            else if(i_points) {
                //assegnazione
              //  ris[i].x() = i1.x() ;
             //   ris[i].y() = i1.y() ;
            }


            Circle c1(dist,  hash_group[i].x_schedina,  hash_group[i].y_schedina);
            cerchi.push_back(c1);
        }

    }

    float valx=0;
    float valy=0;
    //for finale per calcolare il risultato
    for(int i=0;i<ris.size();i++)
    {
        valx=valx+ris[i].x();
        valy=valy+ris[i].y();
    }
    valx=valx/(ris.size()-2);
    valy=valy/(ris.size()-2);
    //PositionData output;
    //output.x()=valx;
    //output.y()=valy;
    //



}