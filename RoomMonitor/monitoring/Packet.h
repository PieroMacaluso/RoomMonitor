#ifndef ROOMMONITOR_PACKET_H
#define ROOMMONITOR_PACKET_H

#include <string>
#include <deque>
#include <vector>
#include "PositionData.h"
#include "Point2d.h"
#include <sstream>

class Packet {
    /**
     * Stringa MAC dispositivo
     */
    std::string macPeer;

    /**
     * ID Schedina
     */
    int id_schedina;
    /**
     * RSSI
     */
    int rssi;

    /**
     * CRC 32bit del pacchetto
     */
    std::string fcs;
    /**
     * Timestamp del pacchetto
     */
    uint32_t timestamp;
    /**
     * SSID del pacchetto, se disponibile
     */
    std::string ssid;
    /**
     * Dati sulla posizione
     */
    PositionData positionData;

public:
    /**
     * Costruttore pacchetto
     * @param id_schedina   id schedina
     * @param fcs           crc 32 bit
     * @param rssi          rssi dBm
     * @param mac           string Mac address ddispositivo rilevato
     * @param timestamp     timestamp dispositivo
     * @param ssid          ssid, se presente
     */
    Packet(int id_schedina, std::string fcs, int rssi, std::string &mac, uint32_t timestamp, std::string &ssid);

    /**
     * Operatore << per stampare a video per debug il pacchetto
     * @param os        output stream
     * @param pk        pacchetto
     * @return          std::ostream
     */
    friend std::ostream &operator<<(std::ostream &os, const Packet &pk);

    /**
     * Get Mac del dispositivo
     * @return std::string      mac dispositivo
     */
    const std::string &getMacPeer() const;

    /**
     * Getter ID Schedina
     * @return int      ID schedina
     */
    int getIdSchedina() const;

    /**
     * Getter RSSI
     * @return int      RSSI
     */
    int getRssi() const;

    /**
     * Getter FCS
     * @return std::string FCS
     */
    const std::string &getFcs() const;

    /**
     * Getter Timestamp
     * @return  uint32_t    timestamp
     */
    uint32_t getTimestamp() const;

    /**
     * Getter SSID
     * @return std::string  SSID string
     */
    const std::string &getSsid() const;

    /**
     * Setter position
     * @param pos       PositionData
     */
    void setPosition(PositionData pos);

    /**
     * Getter Coordinate x
     * @return double       Pos X
     */
    double getX();

    /**
     * Getter Coordinate y
     * @return double       Pos Y
     */
    double getY();

    /**
     * Getter Coordinate X and Y
     * @return Point2d      Point
     */
    Point2d getPoint();

    /**
     * Operator < per comparazione
     * @param lhs       pacchetto di sinistra
     * @param rhs       pacchetto di destra
     * @return bool
     */
    friend bool operator<(const Packet &lhs, const Packet &rhs);
};


#endif //ROOMMONITOR_PACKET_H
