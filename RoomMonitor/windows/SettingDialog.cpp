//
// Created by pieromack on 10/09/19.
//

#include "SettingDialog.h"
#include "../monitoring/Board.h"
#include <QDebug>
#include <QtWidgets/QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlField>
#include <QtSql/QSqlDriver>



SettingDialog::SettingDialog() {
    qRegisterMetaTypeStreamOperators<QList<QStringList>>("Stuff");
    // Inizializzazione Dialogo con Impostazioni e riempimento con i dati.
    settingCheckUp();
    ui.setupUi(this);
    setupConnect();
    ui.aEdit->setText(s.value("monitor/A").toString());
    ui.aEdit->setValidator(new QIntValidator());
    ui.nEdit->setText(s.value("monitor/n").toString());
    ui.nEdit->setValidator(new QIntValidator());
    ui.widthEdit->setText(s.value("room/width").toString());
    ui.widthEdit->setValidator(new QDoubleValidator());
    ui.heightEdit->setText(s.value("room/height").toString());
    ui.heightEdit->setValidator(new QDoubleValidator());
    ui.portServerEdit->setText(s.value("room/port").toString());
    ui.portServerEdit->setValidator(new QIntValidator());
    ui.hostEdit->setText(s.value("database/host").toString());
    ui.dbEdit->setText(s.value("database/name").toString());
    ui.portEdit->setText(s.value("database/port").toString());
    ui.portEdit->setValidator(new QIntValidator());
    ui.userEdit->setText(s.value("database/user").toString());
    ui.passEdit->setText(s.value("database/pass").toString());
    ui.tableEdit->setText(s.value("database/table").toString());
    boardList = s.value("room/boards").value<QList<QStringList>>();
    initializeBoardList();
}

void SettingDialog::settingCheckUp() {
    QSettings su{};
    // Impostazioni iniziali, se non sono mai state configurate
    /* TODO: verificare che possano essere spostate nel programma principale per funzionare di base senza per forza
     * aprire le impostazioni
     */
    qRegisterMetaTypeStreamOperators<QList<QStringList>>("Stuff");
    if (su.value("monitor/A").isNull())
        su.setValue("monitor/A", 3);
    if (su.value("monitor/n").isNull()) su.setValue("monitor/n", -55);
    if (su.value("room/width").isNull()) su.setValue("room/width", 10);
    if (su.value("room/height").isNull()) su.setValue("room/height", 10);
    if (su.value("room/port").isNull()) su.setValue("room/port", 27015);
    if (su.value("room/boards").isNull()) {
        QList<QStringList> data{{"0", "1.2", "1.0"},
                                {"1", "1.3", "2.0"}};
        su.setValue("room/boards", QVariant::fromValue(data));
    }
    if (su.value("database/host").isNull()) su.setValue("database/host", "localhost");
    if (su.value("database/name").isNull()) su.setValue("database/name", "data");
    if (su.value("database/port").isNull()) su.setValue("database/port", 3306);
    if (su.value("database/user").isNull()) su.setValue("database/user", "root");
    if (su.value("database/pass").isNull()) su.setValue("database/pass", "NewRoot12Kz");
    if (su.value("database/table").isNull()) su.setValue("database/table", "stanza");


    // TODO: gestione vettore con schedine e relative posizioni
}

void SettingDialog::setupConnect() {
    connect(ui.buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &SettingDialog::apply);
    connect(ui.removeBoard, &QPushButton::clicked, this, &SettingDialog::removeSelected);
    connect(ui.addBoard, &QPushButton::clicked, this, &SettingDialog::openDialogAdd);
    connect(ui.modBoard, &QPushButton::clicked, this, &SettingDialog::openDialogMod);
    connect(ui.boardTable, &QTableWidget::itemSelectionChanged, [&]() {
        if (ui.boardTable->selectedItems().size() == 0) {
            ui.modBoard->setDisabled(true);
            ui.removeBoard->setDisabled(true);
        } else {
            ui.modBoard->setDisabled(false);
            ui.removeBoard->setDisabled(false);
        }
    });


}

void SettingDialog::initializeBoardList() {
    /* SETUP TABLE */
    ui.boardTable->setColumnCount(3);
    QStringList h;
    h << "ID" << "X" << "Y";
    ui.boardTable->setHorizontalHeaderLabels(h);
    ui.boardTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.boardTable->verticalHeader()->hide();
    ui.boardTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.boardTable->setSelectionMode(QHeaderView::SelectionMode::SingleSelection);
    ui.boardTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.modBoard->setDisabled(true);
    ui.removeBoard->setDisabled(true);
    for (auto board: boardList) {
        int i = ui.boardTable->rowCount();
        ui.boardTable->insertRow(ui.boardTable->rowCount());
        ui.boardTable->setItem(i, 0, new QTableWidgetItem(board[0]));
        ui.boardTable->setItem(i, 1, new QTableWidgetItem(board[1]));
        ui.boardTable->setItem(i, 2, new QTableWidgetItem(board[2]));
    }
    connect(ui.boardTable, &QTableWidget::cellChanged, this, &SettingDialog::elementChanged);


}

void SettingDialog::elementChanged(int row, int column) {
    if (column == 0) {
        for (int i = 0; i < ui.boardTable->rowCount(); i++) {
            if (i == row) continue;
            if (ui.boardTable->item(row, column)->text() == ui.boardTable->item(i, column)->text()) {
                qDebug() << "ERRORE DUPLICATO";
                ui.boardTable->item(row, column)->setText(boardList[row][column]);
                return;
            }

        }


    }

//    boardList[row][column] = ui.boardTable->item(row, column)->text();
}

void SettingDialog::removeSelected() {
    int rr = ui.boardTable->selectedItems().first()->row();
    ui.boardTable->removeRow(rr);
}

void SettingDialog::openDialogAdd() {

    QDialog add;
    addBoardDialog.setupUi(&add);
    add.setModal(true);
    setupAddBoard();
    if (add.exec()) {
        if (addBoardDialog.idEdit->text().isEmpty() || addBoardDialog.xEdit->text().isEmpty() ||
            addBoardDialog.yEdit->text().isEmpty()) {
            QMessageBox msgBox;
            msgBox.setText("Inserimento non valido, riprovare!");
            msgBox.exec();
            return;
        }
        for (int i = 0; i < ui.boardTable->rowCount(); i++) {
            if (addBoardDialog.idEdit->text() == ui.boardTable->item(i, 0)->text()) {
                QMessageBox msgBox;
                msgBox.setText("Id già presente, riprovare!");
                msgBox.exec();
                return;
            }
        }

        addBoard(addBoardDialog.idEdit->text(), addBoardDialog.xEdit->text(), addBoardDialog.yEdit->text());
    }


}

void SettingDialog::openDialogMod() {

    QDialog add;
    modBoardDialog.setupUi(&add);
    add.setModal(true);
    QStringList list;
    for (auto el : ui.boardTable->selectedItems()) {
        list.append(el->text());
    }
    modBoardDialog.idEdit->setText(list[0]);
    modBoardDialog.xEdit->setText(list[1]);
    modBoardDialog.yEdit->setText(list[2]);
    setupModBoard();
    if (add.exec()) {
        if (modBoardDialog.idEdit->text().isEmpty() || modBoardDialog.xEdit->text().isEmpty() ||
            modBoardDialog.yEdit->text().isEmpty()) {
            QMessageBox msgBox;
            msgBox.setText("Inserimento non valido, riprovare!");
            msgBox.exec();
            return;
        }
        for (int i = 0; i < ui.boardTable->rowCount(); i++) {
            if (modBoardDialog.idEdit->text() == ui.boardTable->item(i, 0)->text() &&
                modBoardDialog.idEdit->text() != list[0]) {
                QMessageBox msgBox;
                msgBox.setText("Id già presente, riprovare!");
                msgBox.exec();
                return;
            }
        }
        int i = ui.boardTable->selectedItems().first()->row();
        ui.boardTable->setItem(i, 0, new QTableWidgetItem(modBoardDialog.idEdit->text()));
        ui.boardTable->setItem(i, 1, new QTableWidgetItem(modBoardDialog.xEdit->text()));
        ui.boardTable->setItem(i, 2, new QTableWidgetItem(modBoardDialog.yEdit->text()));
    }


}

void SettingDialog::apply() {
    s.setValue("monitor/A", ui.aEdit->text().toFloat());
    s.setValue("monitor/n", ui.nEdit->text().toFloat());
    s.setValue("room/width", ui.widthEdit->text().toFloat());
    s.setValue("room/height", ui.heightEdit->text().toFloat());
    s.setValue("room/port", ui.portServerEdit->text().toInt());
    s.setValue("database/host", ui.hostEdit->text());
    s.setValue("database/name", ui.dbEdit->text());
    s.setValue("database/port", ui.portEdit->text());
    s.setValue("database/user", ui.userEdit->text());
    s.setValue("database/pass", ui.passEdit->text());
    qRegisterMetaTypeStreamOperators<QList<QStringList>>("Stuff");
    QList<QStringList> b;
    for (int i = 0; i < ui.boardTable->rowCount(); i++) {
        b.append({ui.boardTable->item(i, 0)->text(), ui.boardTable->item(i, 1)->text(),
                  ui.boardTable->item(i, 2)->text()});
    }
    s.setValue("room/boards", QVariant::fromValue(b));

    if (ui.resetCheck->isChecked()) {
        resetDB();
    }
    this->close();

}

void SettingDialog::addBoard(const QString &id, const QString &x, const QString &y) {
    int i = ui.boardTable->rowCount();
//    boardList.append({id, x, y});
    ui.boardTable->insertRow(ui.boardTable->rowCount());
    ui.boardTable->setItem(i, 0, new QTableWidgetItem(id));
    ui.boardTable->setItem(i, 1, new QTableWidgetItem(x));
    ui.boardTable->setItem(i, 2, new QTableWidgetItem(y));

}

void SettingDialog::setupAddBoard() {
    addBoardDialog.idEdit->setValidator(new QIntValidator());
    addBoardDialog.xEdit->setValidator(new QDoubleValidator());
    addBoardDialog.yEdit->setValidator(new QDoubleValidator());
    checkAddEdits();
    connect(addBoardDialog.idEdit, &QLineEdit::textChanged, this, &SettingDialog::checkAddEdits);
    connect(addBoardDialog.xEdit, &QLineEdit::textChanged, this, &SettingDialog::checkAddEdits);
    connect(addBoardDialog.yEdit, &QLineEdit::textChanged, this, &SettingDialog::checkAddEdits);
}

void SettingDialog::checkAddEdits() {
    bool id;
    bool x;
    bool y;
    addBoardDialog.idEdit->text().toInt(&id);
    addBoardDialog.xEdit->text().toDouble(&x);
    addBoardDialog.yEdit->text().toDouble(&y);

    addBoardDialog.buttonBox->button(QDialogButtonBox::Ok)->setDisabled(!(id && x && y));
}


void SettingDialog::setupModBoard() {
    modBoardDialog.idEdit->setValidator(new QIntValidator());
    modBoardDialog.xEdit->setValidator(new QDoubleValidator());
    modBoardDialog.yEdit->setValidator(new QDoubleValidator());

    checkModEdits();
    connect(modBoardDialog.idEdit, &QLineEdit::textChanged, this, &SettingDialog::checkModEdits);
    connect(modBoardDialog.xEdit, &QLineEdit::textChanged, this, &SettingDialog::checkModEdits);
    connect(modBoardDialog.yEdit, &QLineEdit::textChanged, this, &SettingDialog::checkModEdits);
}

void SettingDialog::checkModEdits() {
    bool id;
    bool x;
    bool y;
    modBoardDialog.idEdit->text().toInt(&id);
    modBoardDialog.xEdit->text().toDouble(&x);
    modBoardDialog.yEdit->text().toDouble(&y);

    modBoardDialog.buttonBox->button(QDialogButtonBox::Ok)->setDisabled(!(id && x && y));
}

void SettingDialog::resetDB() {
    {
        //TODO: ALARM SQL INJECTION!
        QSqlDatabase db;
        db = QSqlDatabase::addDatabase("QMYSQL");
        db.setHostName(s.value("database/host").toString());
        db.setDatabaseName(s.value("database/name").toString());
        db.setPort(s.value("database/port").toInt());
        db.setUserName(s.value("database/user").toString());
        db.setPassword(s.value("database/pass").toString());

        qDebug() << db.driver()->hasFeature(QSqlDriver::PreparedQueries);

        if (!db.open()) {
            qDebug() << db.lastError();
            return;
        }

        QSqlQuery query{};
        query.prepare("DROP TABLE IF EXISTS " + s.value("database/table").toString() + ";");

        if (!query.exec()) {
            qDebug() << query.lastError();
        }
        query.prepare("CREATE TABLE " + s.value("database/table").toString() + " ("
                      "id_packet int auto_increment, "
                      "hash_fcs varchar(8) NOT NULL, "
                      "id_room int NOT NULL, "
                      "id_board int NOT NULL, "
                      "mac_addr varchar(17) NOT NULL, "
                      "pos_x REAL(5,2) NOT NULL, "
                      "pos_y REAL(5,2) NOT NULL, "
                      "timestamp timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, "
                      "ssid varchar(64) NOT NULL, "
                      "hidden int NOT NULL, "
                      "PRIMARY KEY (id_packet) "
                      ");");

        if (!query.exec()) {
            qDebug() << query.lastError();
        }
        db.close();
    }
    QSqlDatabase::removeDatabase("qt_sql_default_connection");

}
