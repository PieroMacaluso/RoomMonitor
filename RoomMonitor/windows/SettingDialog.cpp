//
// Created by pieromack on 10/09/19.
//

#include "SettingDialog.h"
#include "../monitoring/Board.h"
#include <QDebug>


SettingDialog::SettingDialog() {
    qRegisterMetaTypeStreamOperators<QList<QStringList>>("Stuff");
    // Inizializzazione Dialogo con Impostazioni e riempimento con i dati.
    settingCheckUp();
    ui.setupUi(this);
    setupConnect();
    ui.aEdit->setText(s.value("monitor/A").toString());
    ui.nEdit->setText(s.value("monitor/n").toString());
    ui.widthEdit->setText(s.value("room/width").toString());
    ui.heightEdit->setText(s.value("room/height").toString());
    ui.hostEdit->setText(s.value("database/host").toString());
    ui.dbEdit->setText(s.value("database/name").toString());
    ui.portEdit->setText(s.value("database/port").toString());
    ui.userEdit->setText(s.value("database/user").toString());
    ui.passEdit->setText(s.value("database/pass").toString());
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


    // TODO: gestione vettore con schedine e relative posizioni
}

void SettingDialog::setupConnect() {
    connect(ui.buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &SettingDialog::apply);
    connect(ui.removeBoard, &QPushButton::clicked, this, &SettingDialog::removeSelected);
    connect(ui.addBoard, &QPushButton::clicked, this, &SettingDialog::openDialogAdd);

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

    boardList[row][column] = ui.boardTable->item(row, column)->text();
}

void SettingDialog::removeSelected() {
    for (auto i : ui.boardTable->selectedItems()) {
        boardList.removeAt(i->row());
        ui.boardTable->removeRow(i->row());
    }

}

void SettingDialog::openDialogAdd() {

    QDialog add;
    addBoardDialog.setupUi(&add);
    add.setModal(true);
    if (add.exec()){
        addBoard(addBoardDialog.idEdit->text(), addBoardDialog.xEdit->text(), addBoardDialog.yEdit->text());
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
    s.setValue("room/boards", QVariant::fromValue(boardList));

    this->close();

}

void SettingDialog::addBoard(const QString& id, const QString& x, const QString& y) {
    int i = ui.boardTable->rowCount();
    boardList.append({id, x, y});
    ui.boardTable->insertRow(ui.boardTable->rowCount());
    ui.boardTable->setItem(i, 0, new QTableWidgetItem(id));
    ui.boardTable->setItem(i, 1, new QTableWidgetItem(x));
    ui.boardTable->setItem(i, 2, new QTableWidgetItem(y));

}
