//
// Created by pieromack on 04/11/19.
//

#include "QMapSlider.h"
#include "QTimeSlider.h"

void QMapSlider::mouseMoveEvent(QMouseEvent *ev) {
    emitDateMax(this->getMapIndex(this->value()));
    QSlider::mouseMoveEvent(ev);
}

QMapSlider::QMapSlider(QWidget *parent) : QSlider(parent) {

}

const std::map<QDateTime, std::vector<LastMac>> &QMapSlider::getMap() const {
    return map;
}

void QMapSlider::setMap(const std::map<QDateTime, std::vector<LastMac>> &mapOut) {
    QMapSlider::map = mapOut;
    this->setMaximum(mapOut.size()-1);
    for (auto it = mapOut.begin(); it != mapOut.end(); ++it) {
        keyMap.push_back(it->first);
        map.insert(std::make_pair(it->first, it->second));
    }
    this->setSliderPosition(0);
    emit initialized();
}

std::vector<LastMac> QMapSlider::getMapIndex(int value) {
    auto el = this->map.find(keyMap[value]);
    if (el != this->map.end()){
        return el->second;
    } else {
        // Ritorno .end() se non sono presenti dati
        std::vector<LastMac> empty{};
        return empty;
    }
}
QDateTime QMapSlider::getKeyIndex(int value) {
    return keyMap[value];
}



