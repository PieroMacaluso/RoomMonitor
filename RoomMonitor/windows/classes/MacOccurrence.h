#ifndef ROOMMONITOR_MACOCCURRENCE_H
#define ROOMMONITOR_MACOCCURRENCE_H


#include <QtCore/QString>

class MacOccurrence {
    /**
     * MAC String
     */
    QString mac;
    /**
     * Number of occurrences
     */
    int number_of_occurrences;

public:

    /**
     * Costruttore parametrizzaro
     * @param mac
     * @param numberOfOccurrences
     */
    MacOccurrence(const QString &mac, int numberOfOccurrences);

    /**
     * Costruttore vuoto
     */
    MacOccurrence();

    /**
     * Getter MAC
     * @return MAC string
     */
    const QString &getMac() const;

    /**
     * Getter number of occurences
     * @return
     */
    int getNumberOfOccurrences() const;


};

/**
 * Operatore < per comparazione
 * @param e1
 * @param e2
 * @return
 */
inline bool operator<(const MacOccurrence &e1, const MacOccurrence &e2)
{
    if (e1.getNumberOfOccurrences() != e2.getNumberOfOccurrences())
        return e1.getNumberOfOccurrences() > e2.getNumberOfOccurrences();
    return e1.getMac() > e2.getMac();
}


#endif //ROOMMONITOR_MACOCCURRENCE_H
