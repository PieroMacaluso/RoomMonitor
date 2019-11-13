//
// Created by pieromack on 13/11/19.
//

#include "Query.h"

const QString Query::DROP_TABLE_PACKET = "DROP TABLE IF EXISTS %1;";
const QString Query::CREATE_TABLE_PACKET = "CREATE TABLE %1 ("
                                           "id_packet int auto_increment, "
                                           "hash_fcs varchar(8) NOT NULL, "
                                           "mac_addr varchar(17) NOT NULL, "
                                           "pos_x REAL(5,2) NOT NULL, "
                                           "pos_y REAL(5,2) NOT NULL, "
                                           "timestamp timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, "
                                           "ssid varchar(64) NOT NULL, "
                                           "hidden int NOT NULL, "
                                           "PRIMARY KEY (id_packet) "
                                           ");";
const QString Query::DROP_TABLE_BOARD = "DROP TABLE IF EXISTS board_%1;";
const QString Query::CREATE_TABLE_BOARD = "CREATE TABLE board_%1 ("
                                          "id_board int NOT NULL, "
                                          "pos_x REAL(5,2) NOT NULL, "
                                          "pos_y REAL(5,2) NOT NULL,"
                                          "a int NOT NULL, "
                                          "PRIMARY KEY (id_board) "
                                          ");";
const QString Query::SELECT_ALL_PACKET = "SELECT * FROM %1;";
const QString Query::SELECT_ALL_BOARD = "SELECT id_board, pos_x, pos_y, a FROM board_%1;";
const QString Query::INSERT_BOARD = "INSERT INTO board_%1 (id_board, pos_x, pos_y, a) VALUES (:id_board, :pos_x, :pos_y, :a);";
