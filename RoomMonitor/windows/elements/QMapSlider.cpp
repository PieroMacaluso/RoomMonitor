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

void QMapSlider::setMap(const std::map<QDateTime, std::vector<LastMac>> &mapOut) {
    QMapSlider::map.clear();
    QMapSlider::map = mapOut;
    keyMap.clear();
    int max = static_cast<int>(mapOut.size()) - 1;
    for (auto &it: mapOut) {
        keyMap.push_back(it.first);
        map.insert(std::make_pair(it.first, it.second));
    }
    this->setMinimum(0);
    this->setMaximum(max);
    this->setSliderPosition(0);
    emit initialized();
}

std::vector<LastMac> QMapSlider::getMapIndex(int value) {
    auto el = this->map.find(keyMap[value]);
    if (el != this->map.end()) {
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

bool QMapSlider::isMapEmpty() {
    return map.empty();
}



