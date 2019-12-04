#ifndef ROOMMONITOR_POSITIONDATA_H
#define ROOMMONITOR_POSITIONDATA_H


#include <iostream>
#include <sstream>

class PositionData {
    /**
     * Numero di pacchetti inseriti nella struttura
     */
    int n_packets;
    /**
     * Coordinata X
     */
    double x;
    /**
     * Coordinata Y
     */
    double y;
public:
    /**
     * Costruttore parametrizzato
     * @param x     pos x
     * @param y     pos y
     */
    PositionData(double x, double y) : x(x), y(y), n_packets(1) {}

    /**
     * Costruttore vuoto
     */
    PositionData() : x(0), y(0), n_packets(0) {};

    /**
     * Aggiunta pacchetto con media mobile
     * @param new_x
     * @param new_y
     */
    void addPacket(double new_x, double new_y);

    /**
     * Operatore << per stampe di debug
     * @param os    output stream
     * @param a     position data
     * @return      output stream
     */
    friend std::ostream &operator<<(std::ostream &os, const PositionData &a);


    /**
     * Getter x
     * @return  pos x
     */
    double getX() const {
        return x;
    }

    /**
     * Getter y
     * @return pos y
     */
    double getY() const {
        return y;
    }

    /**
     * Getter stringa posizione
     * @return stringa posizione
     */
    std::string getStringPosition() {
        std::ostringstream strX;
        std::ostringstream strY;
        strX << x;
        strY << y;
        return strX.str() + "," + strY.str();
    }

    static PositionData positionDataNull();
};

/**
 * Operator == per comparazione booleana
 * @param e1 primo elemento
 * @param e2 secondo elemento
 * @return true se uguali, false altrimenti
 */
inline bool operator==(const PositionData &e1, const PositionData &e2) {
    return e1.getX() == e2.getX() && e1.getY() == e2.getY();
}


#endif //ROOMMONITOR_POSITIONDATA_H
