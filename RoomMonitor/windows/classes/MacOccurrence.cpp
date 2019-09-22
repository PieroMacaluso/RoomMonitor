//
// Created by pieromack on 22/09/19.
//

#include "MacOccurrence.h"

const QString &MacOccurrence::getMac() const {
    return mac;
}

int MacOccurrence::getNumberOfOccurrences() const {
    return number_of_occurrences;
}

MacOccurrence::MacOccurrence(const QString &mac, int numberOfOccurrences) : mac(mac), number_of_occurrences(
        numberOfOccurrences) {}

MacOccurrence::MacOccurrence() {

}
