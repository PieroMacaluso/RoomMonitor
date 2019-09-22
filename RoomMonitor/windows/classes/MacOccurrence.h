//
// Created by pieromack on 22/09/19.
//

#ifndef ROOMMONITOR_MACOCCURRENCE_H
#define ROOMMONITOR_MACOCCURRENCE_H


#include <QtCore/QString>

class MacOccurrence {
    QString mac;
    int number_of_occurrences;

public:
    MacOccurrence(const QString &mac, int numberOfOccurrences);
    MacOccurrence();


    const QString &getMac() const;

    int getNumberOfOccurrences() const;


};

inline bool operator<(const MacOccurrence &e1, const MacOccurrence &e2)
{
    if (e1.getNumberOfOccurrences() != e2.getNumberOfOccurrences())
        return e1.getNumberOfOccurrences() > e2.getNumberOfOccurrences();
    return e1.getMac() > e2.getMac();
}


#endif //ROOMMONITOR_MACOCCURRENCE_H
