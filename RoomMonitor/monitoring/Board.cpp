//
// Created by pieromack on 11/08/19.
//

#include <QtCore/QSettings>
#include "Board.h"

int Board::getId() const {
    return id;
}

const Point2d &Board::getCoord() const {
    return coord;
}

std::map<int, Board> Board::extract_from_setting() {
    QSettings s{};
    std::map<int, Board> result;
    QList<QStringList> list = s.value("room/boards").value<QList<QStringList>>();
    for (auto b : list){
        int id = b[0].toInt();
        double posx = b[1].toDouble();
        double posy= b[2].toDouble();
        Board board{id, posx, posy};
        result.insert(std::make_pair(id, board));
    }


}
