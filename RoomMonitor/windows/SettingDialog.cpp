//
// Created by pieromack on 10/09/19.
//

#include <QtWidgets/QTableWidgetItem>
#include "SettingDialog.h"
#include "../monitoring/Board.h"


SettingDialog::SettingDialog() {
    // Linea che serve per poter salvare nelle impostazioni una QList<QStringList>
    qRegisterMetaTypeStreamOperators<QList<QStringList>>("Stuff");
    // Inizializzazione SettingDialog con impostazioni da QSettings.
    settingCheckUp();
    ui.setupUi(this);
    setupConnect();
    ui.aEdit->setText(s.value("monitor/A").toString());
    ui.aEdit->setValidator(new QIntValidator());
    ui.nEdit->setText(s.value("monitor/n").toString());
    ui.nEdit->setValidator(new QIntValidator());
    ui.minEdit->setValue(s.value("monitor/min").toInt());
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
    QSettings su{"VALP", "RoomMonitoring"};
    // Inizializzo le impostazioni, se non sono mai state configurate
    qRegisterMetaTypeStreamOperators<QList<QStringList>>("Stuff");
    if (su.value("monitor/A").isNull())
        su.setValue("monitor/A", 3);
    if (su.value("monitor/n").isNull()) su.setValue("monitor/n", -55);
    if (su.value("monitor/min").isNull()) su.setValue("monitor/min", 3);
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
}

void SettingDialog::setupConnect() {
    // Connect varie
    connect(ui.buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &SettingDialog::apply);
    connect(ui.removeBoard, &QPushButton::clicked, this, &SettingDialog::removeSelected);
    connect(ui.addBoard, &QPushButton::clicked, this, &SettingDialog::openDialogAdd);
    connect(ui.modBoard, &QPushButton::clicked, this, &SettingDialog::openDialogMod);
    connect(ui.toolButton, &QPushButton::clicked,this,&SettingDialog::defaultValues);
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
    // TODO: controlla che la funzione elementChanged serva ancora effettivamente
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
            msgBox.setStandardButtons(QMessageBox::Close);
            msgBox.setWindowTitle("Errore inserimento dati schedina");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setText("Inserimento non valido, riprovare!");
            msgBox.exec();
            return;
        }
        for (int i = 0; i < ui.boardTable->rowCount(); i++) {
            if (addBoardDialog.idEdit->text() == ui.boardTable->item(i, 0)->text()) {
                QMessageBox msgBox;
                msgBox.setText("ID già presente, riprovare!");
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
            msgBox.setStandardButtons(QMessageBox::Close);
            msgBox.setWindowTitle("Errore inserimento dati schedina");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setText("Inserimento non valido, riprovare!");
            return;
        }
        for (int i = 0; i < ui.boardTable->rowCount(); i++) {
            if (modBoardDialog.idEdit->text() == ui.boardTable->item(i, 0)->text() &&
                modBoardDialog.idEdit->text() != list[0]) {
                QMessageBox msgBox;
                msgBox.setStandardButtons(QMessageBox::Close);
                msgBox.setWindowTitle("Errore inserimento dati schedina");
                msgBox.setIcon(QMessageBox::Warning);
                msgBox.setText("ID già presente, riprovare!");
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
    if (!isSettingValid()) return;
    s.setValue("monitor/A", ui.aEdit->text().toFloat());
    s.setValue("monitor/n", ui.nEdit->text().toFloat());
    s.setValue("monitor/min", ui.minEdit->text().toFloat());
    s.setValue("room/width", ui.widthEdit->text().toFloat());
    s.setValue("room/height", ui.heightEdit->text().toFloat());
    s.setValue("room/port", ui.portServerEdit->text().toInt());
    s.setValue("database/host", ui.hostEdit->text());
    s.setValue("database/name", ui.dbEdit->text());
    s.setValue("database/port", ui.portEdit->text());
    s.setValue("database/user", ui.userEdit->text());
    s.setValue("database/pass", ui.passEdit->text());
    s.setValue("database/table", ui.tableEdit->text());
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

void SettingDialog::defaultValues(){

    // Ripristino informazioni iniziali

    s.setValue("monitor/A", -55);
    s.setValue("monitor/n", 3);
    s.setValue("monitor/min", 3);
    s.setValue("room/width", 10);
    s.setValue("room/height", 10);
    s.setValue("room/port", 27015);

    QList<QStringList> data{{"0", "1.2", "1.0"},
                            {"1", "1.3", "2.0"}};
    s.setValue("room/boards", QVariant::fromValue(data));

    s.setValue("database/host", "localhost");
    s.setValue("database/name", "data");
    s.setValue("database/port", 3306);
    s.setValue("database/user", "root");
    s.setValue("database/pass", "NewRoot12Kz");
    s.setValue("database/table", "stanza");
    qRegisterMetaTypeStreamOperators<QList<QStringList>>("Stuff");
    this->close();
}

void SettingDialog::resetDB() {
    //TODO: ALARM SQL INJECTION?!? Controllare bene
    QSqlDatabase::removeDatabase(QSqlDatabase::database().connectionName());
    QSqlDatabase db{};
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(s.value("database/host").toString());
    db.setDatabaseName(s.value("database/name").toString());
    db.setPort(s.value("database/port").toInt());
    db.setUserName(s.value("database/user").toString());
    db.setPassword(s.value("database/pass").toString());


    if (!db.open()) {
        qDebug() << db.lastError();
        return;
    }
    qDebug() << db.driver()->hasFeature(QSqlDriver::PreparedQueries);


    QSqlQuery query{db};
    query.prepare("DROP TABLE IF EXISTS " + s.value("database/table").toString() + ";");

    if (!query.exec()) {
        qDebug() << query.lastError();
    }
    query.prepare("CREATE TABLE " + s.value("database/table").toString() +
                  " ("
                  "id_packet int auto_increment, "
                  "hash_fcs varchar(8) NOT NULL, "
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

bool SettingDialog::isSettingValid() {

    // TODO: Controllare e testare validazione Input Impostazioni
    QString err = "";
    int pos = 0;
    QRegExp port{"^()([1-9]|[1-5]?[0-9]{2,4}|6[1-4][0-9]{3}|65[1-4][0-9]{2}|655[1-2][0-9]|6553[1-5])$"};
    QRegExp host{
            "^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\\-]*[a-zA-Z0-9])\\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\\-]*[A-Za-z0-9])$"};
    QRegExp dbName{"^[0-9a-zA-Z$_]+$"};
    QRegExp username{"^[A-Za-z0-9_]{1,64}$"};

    /* STANZA */
    bool x;
    qreal width = ui.widthEdit->text().toDouble(&x);
    if (!x || width <= 0) err += "Larghezza stanza non valida.\n";
    qreal height = ui.heightEdit->text().toDouble(&x);
    if (!x || height <= 0) err += "Altezza stanza non valida.\n";
    QRegExpValidator port_v{port, nullptr};
    QString port_s{ui.portServerEdit->text()};
    if (port_v.validate(port_s, pos) != QValidator::Acceptable) err += "Porta Server non valida.\n";
    if (ui.boardTable->rowCount() < 2) err += "Non sono state installate abbastanza schedine.\n";

    /* MONITORAGGIO */
    int n = ui.nEdit->text().toInt();
    if (n <= 0 || n >= 4) err += "Costante di propagazione non valida.\n";

    int a = ui.aEdit->text().toInt();
    if (a >= 0) err += "Potenza in dB a un metro non valida.\n";

    /* DATABASE */
    QRegExpValidator host_v{host, nullptr};
    QString host_s{ui.hostEdit->text()};
    if (host_v.validate(host_s, pos) != QValidator::Acceptable) err += "Host DB non valido.\n";

    QRegExpValidator dbName_v{dbName, nullptr};
    QString dbName_s{ui.dbEdit->text()};
    if (dbName_v.validate(dbName_s, pos) != QValidator::Acceptable) err += "Nome DB non valido.\n";

    QString portdb_s{ui.portEdit->text()};
    if (port_v.validate(portdb_s, pos) != QValidator::Acceptable) err += "Porta DB non valida.\n";

    QRegExpValidator username_v{username, nullptr};
    QString username_s{ui.userEdit->text()};
    if (username_v.validate(username_s, pos) != QValidator::Acceptable) err += "Nome utente non valido.\n";

    if (ui.passEdit->text() == "") err += "Password Utente non valida.\n";
    QString table_s{ui.tableEdit->text()};
    if (username_v.validate(table_s, pos) != QValidator::Acceptable) err += "Nome tabella non valido.\n";

    if (err == "") {
        return true;
    } else {
        QMessageBox msgBox;
        msgBox.setStandardButtons(QMessageBox::Close);
        msgBox.setWindowTitle("Errore impostazioni");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(err);
        msgBox.exec();
        return false;
    }
}
