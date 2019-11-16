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
/**
 * Deprecated
 * @return
 */
std::map<int, Board> Board::extract_from_setting() {
    QSettings s{"VALP", "RoomMonitoring"};
    std::map<int, Board> result;
    auto list = s.value("room/boards").value<QList<QStringList>>();
    for (auto b : list) {
        int id = b[0].toInt();
        double posx = b[1].toDouble();
        double posy = b[2].toDouble();
        int A = b[3].toInt();
        Board board{id, posx, posy, A};
        result.insert(std::make_pair(id, board));
    }
    return result;

}

int Board::getA() const {
    return A;
}
