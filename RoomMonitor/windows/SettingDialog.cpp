//
// Created by pieromack on 10/09/19.
//

#include "SettingDialog.h"

SettingDialog::SettingDialog() {
    // Inizializzazione SettingDialog con impostazioni da QSettings.
    settingCheckUp();
    ui.setupUi(this);
    setupConnect();
    initializeBoardList();
    if (SettingDialog::settingCheckUp()) this->compileValues();
}

bool SettingDialog::settingCheckUp() {
    // Inizializzo le impostazioni, se non sono mai state configurate
    QSettings su{Utility::ORGANIZATION, Utility::APPLICATION};
    QVariant val = su.value("first_time");
    if (val.isValid()) {
        bool error = false;
        QSqlDatabase db = Utility::getDB(error);
        if (error) return false;
        if (!Utility::testTable(db)) return false;
    }
    return val.isValid();
}

void SettingDialog::setupConnect() {
    // Connect varie
    connect(ui.buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &SettingDialog::apply);
    connect(ui.removeBoard, &QPushButton::clicked, this, &SettingDialog::removeSelected);
    connect(ui.addBoard, &QPushButton::clicked, this, &SettingDialog::openDialogAdd);
    connect(ui.modBoard, &QPushButton::clicked, this, &SettingDialog::openDialogMod);
    connect(ui.restoreBoardButton, &QPushButton::clicked, this, &SettingDialog::restoreBoards);
    connect(ui.buttonBox->button(QDialogButtonBox::RestoreDefaults), &QPushButton::clicked, this,
            &SettingDialog::defaultValues);
    connect(ui.boardTable, &QTableWidget::itemSelectionChanged, [&]() {
        if (ui.boardTable->selectedItems().empty()) {
            ui.modBoard->setDisabled(true);
            ui.removeBoard->setDisabled(true);
        } else {
            ui.modBoard->setDisabled(false);
            ui.removeBoard->setDisabled(false);
        }
    });

    connect(ui.secretEcho, &QToolButton::pressed, [&]() {
        ui.secretEdit->setEchoMode(QLineEdit::Normal);
    });
    connect(ui.secretEcho, &QToolButton::released, [&]() {
        ui.secretEdit->setEchoMode(QLineEdit::Password);
    });
    connect(ui.passEcho, &QToolButton::pressed, [&]() {
        ui.passEdit->setEchoMode(QLineEdit::Normal);
    });

    connect(ui.passEcho, &QToolButton::released, [&]() {
        ui.passEdit->setEchoMode(QLineEdit::Password);
    });

    connect(ui.initializeButton, &QPushButton::clicked, [&]() {
        if (this->isSettingValid() && Utility::yesNoMessage(this, "Creazione DB",
                                                            "Sei sicuro di voler creare/sovrascrivere la tabella indicata?")) {
            this->resetDB();
        }
    });
}

void SettingDialog::initializeBoardList() {
    ui.boardTable->clear();
    ui.boardTable->setRowCount(0);
    /* SETUP TABLE */
    ui.boardTable->setColumnCount(4);
    QStringList h;
    h << "ID" << "X" << "Y" << "A";
    ui.boardTable->setHorizontalHeaderLabels(h);
    ui.boardTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.boardTable->verticalHeader()->hide();
    ui.boardTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui.boardTable->setSelectionMode(QHeaderView::SelectionMode::SingleSelection);
    ui.boardTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.modBoard->setDisabled(true);
    ui.removeBoard->setDisabled(true);
}

void SettingDialog::fillBoardList() {
    std::vector<Board> boardList = Utility::getBoards();
    ui.boardTable->setRowCount(0);
    for (auto board: boardList) {
        int i = ui.boardTable->rowCount();
        ui.boardTable->insertRow(ui.boardTable->rowCount());
        ui.boardTable->setItem(i, 0, new QTableWidgetItem(QString::number(board.getId())));
        ui.boardTable->setItem(i, 1, new QTableWidgetItem(QString::number(board.getCoord().x())));
        ui.boardTable->setItem(i, 2, new QTableWidgetItem(QString::number(board.getCoord().y())));
        ui.boardTable->setItem(i, 3, new QTableWidgetItem(QString::number(board.getA())));

    }
}

void SettingDialog::fillBoardListDefault() {
    Board one{0, 0, 0, -55};
    Board two{1, 10, 10, -55};
    std::vector<Board> boardList;
    boardList.push_back(one);
    boardList.push_back(two);
    ui.boardTable->setRowCount(0);
    for (auto board: boardList) {
        int i = ui.boardTable->rowCount();
        ui.boardTable->insertRow(ui.boardTable->rowCount());
        ui.boardTable->setItem(i, 0, new QTableWidgetItem(QString::number(board.getId())));
        ui.boardTable->setItem(i, 1, new QTableWidgetItem(QString::number(board.getCoord().x())));
        ui.boardTable->setItem(i, 2, new QTableWidgetItem(QString::number(board.getCoord().y())));
        ui.boardTable->setItem(i, 3, new QTableWidgetItem(QString::number(board.getA())));

    }
}

void SettingDialog::removeSelected() {
    int rr = ui.boardTable->selectedItems().first()->row();
    ui.boardTable->removeRow(rr);
}

void SettingDialog::restoreBoards() {
    if (Utility::yesNoMessage(this, Strings::RESTORE_BOARD, Strings::RESTORE_BOARD_MSG)) {
        {
            QSqlDatabase db{};
            db = QSqlDatabase::addDatabase("QMYSQL", "restoreBoard");
            db.setHostName(ui.hostEdit->text());
            db.setDatabaseName(ui.dbEdit->text());
            db.setPort(ui.portEdit->text().toInt());
            db.setUserName(ui.userEdit->text());
            db.setPassword(ui.passEdit->text());
            if (!db.open()) {
                Utility::warningMessage(Strings::ERR_DB, Strings::ERR_DB_MSG, db.lastError().text());
                return;
            }
            QSqlQuery query{db};
            query.prepare(Query::SELECT_ALL_BOARD.arg(ui.tableEdit->text()));
            if (!query.exec()) {
                Utility::warningMessage(Strings::ERR_DB, Strings::ERR_DB_MSG, query.lastError().text());
                return;
            }
            std::vector<Board> res;
            while (query.next()) {
                int id = query.value(0).toInt();
                qreal posx = query.value(1).toDouble();
                qreal posy = query.value(2).toDouble();
                int A = query.value(3).toInt();
                Board b{id, posx, posy, A};
                res.push_back(b);
            }

            ui.boardTable->setRowCount(0);
            for (auto board: res) {
                int i = ui.boardTable->rowCount();
                ui.boardTable->insertRow(ui.boardTable->rowCount());
                ui.boardTable->setItem(i, 0, new QTableWidgetItem(QString::number(board.getId())));
                ui.boardTable->setItem(i, 1, new QTableWidgetItem(QString::number(board.getCoord().x())));
                ui.boardTable->setItem(i, 2, new QTableWidgetItem(QString::number(board.getCoord().y())));
                ui.boardTable->setItem(i, 3, new QTableWidgetItem(QString::number(board.getA())));
            }
            db.close();
            Utility::infoMessage("Ripristino Schedine", "Schedine ripristinate correttamente da database.");
        }
        QSqlDatabase::removeDatabase("restoreBoard");
    }
}


void SettingDialog::openDialogAdd() {
    QDialog add;
    addBoardDialog.setupUi(&add);
    add.setWindowTitle("Aggiungi Schedina");
    add.setModal(true);
//    setupAddBoard();

    while (add.exec()) {
        if (addBoardDialog.idEdit->text().isEmpty() || addBoardDialog.xEdit->text().isEmpty() ||
            addBoardDialog.yEdit->text().isEmpty() || addBoardDialog.aEdit->text().isEmpty()) {
            QMessageBox msgBox;
            msgBox.setStandardButtons(QMessageBox::Close);
            msgBox.setWindowTitle("Errore inserimento dati schedina");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setText("Inserimento non valido, riprovare!");
            msgBox.exec();
            continue;
        }
        bool valid = true;
        for (int i = 0; i < ui.boardTable->rowCount(); i++) {
            if (addBoardDialog.idEdit->text() == ui.boardTable->item(i, 0)->text()) {
                QMessageBox msgBox;
                msgBox.setText("ID già presente, riprovare!");
                msgBox.exec();
                valid = false;
                break;
            }
        }
        if (valid) {
            addBoard(QString::number(addBoardDialog.idEdit->value()), QString::number(addBoardDialog.xEdit->value()),
                     QString::number(addBoardDialog.yEdit->value()),
                     QString::number(addBoardDialog.aEdit->value()));
            break;
        }
    }
}

void SettingDialog::openDialogMod() {
    QDialog add;
    addBoardDialog.setupUi(&add);
    add.setWindowTitle("Modifica Schedina");
    add.setModal(true);
    QStringList list;
    for (auto el : ui.boardTable->selectedItems()) {
        list.append(el->text());
    }
    addBoardDialog.idEdit->setValue(list[0].toInt());
    addBoardDialog.xEdit->setValue(list[1].toDouble());
    addBoardDialog.yEdit->setValue(list[2].toDouble());
    addBoardDialog.aEdit->setValue(list[3].toInt());

    while (add.exec()) {
        if (addBoardDialog.idEdit->text().isEmpty() || addBoardDialog.xEdit->text().isEmpty() ||
            addBoardDialog.yEdit->text().isEmpty() || addBoardDialog.aEdit->text().isEmpty()) {
            QMessageBox msgBox;
            msgBox.setStandardButtons(QMessageBox::Close);
            msgBox.setWindowTitle("Errore inserimento dati schedina");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setText("Inserimento non valido, riprovare!");
            continue;
        }
        bool valid = true;
        for (int i = 0; i < ui.boardTable->rowCount(); i++) {
            if (addBoardDialog.idEdit->text() == ui.boardTable->item(i, 0)->text() &&
                addBoardDialog.idEdit->text() != list[0]) {
                QMessageBox msgBox;
                msgBox.setStandardButtons(QMessageBox::Close);
                msgBox.setWindowTitle("Errore inserimento dati schedina");
                msgBox.setIcon(QMessageBox::Warning);
                msgBox.setText("ID già presente, riprovare!");
                msgBox.exec();
                valid = false;
                break;
            }
        }
        if (valid) {
            int i = ui.boardTable->selectedItems().first()->row();
            ui.boardTable->setItem(i, 0, new QTableWidgetItem(QString::number(addBoardDialog.idEdit->value())));
            ui.boardTable->setItem(i, 1, new QTableWidgetItem(QString::number(addBoardDialog.xEdit->value())));
            ui.boardTable->setItem(i, 2, new QTableWidgetItem(QString::number(addBoardDialog.yEdit->value())));
            ui.boardTable->setItem(i, 3, new QTableWidgetItem(QString::number(addBoardDialog.aEdit->value())));
            break;
        }
    }
}

void SettingDialog::apply() {
    if (!isSettingValid()) return;
    s.setValue("monitor/n", ui.nEdit->value());
    s.setValue("monitor/min", ui.minEdit->value());
    s.setValue("room/width", ui.widthEdit->value());
    s.setValue("room/height", ui.heightEdit->value());
    s.setValue("room/port", ui.portServerEdit->value());
    s.setValue("database/host", ui.hostEdit->text());
    s.setValue("database/name", ui.dbEdit->text());
    s.setValue("database/port", ui.portEdit->value());
    s.setValue("database/user", ui.userEdit->text());
    s.setValue("database/pass", ui.passEdit->text());
    s.setValue("database/table", ui.tableEdit->text());
    s.setValue("Utility/RETRY_STEP_BOARD", ui.retryEdit->value());
    s.setValue("mac/pos/tol", ui.posTolEdit->value());
    s.setValue("mac/pos/peso", ui.posPesoEdit->value());
    s.setValue("mac/time/tol", ui.timeTolEdit->value());
    s.setValue("mac/time/peso", ui.timePesoEdit->value());
    s.setValue("mac/ssid/peso", ui.ssidPesoEdit->value());
    s.setValue("first_time", true);
    s.setValue("secret", ui.secretEdit->text());

    Utility::dropBoards();
    for (int i = 0; i < ui.boardTable->rowCount(); i++) {
        addBoardToDB(ui.boardTable->item(i, 0)->text(), ui.boardTable->item(i, 1)->text(),
                     ui.boardTable->item(i, 2)->text(), ui.boardTable->item(i, 3)->text());
    }
    Utility::setupVariables();
    qInfo() << Strings::SET_CONF;
    this->accept();
}

void SettingDialog::addBoard(const QString &id, const QString &x, const QString &y, const QString &a) {
    int i = ui.boardTable->rowCount();
    ui.boardTable->insertRow(ui.boardTable->rowCount());
    ui.boardTable->setItem(i, 0, new QTableWidgetItem(id));
    ui.boardTable->setItem(i, 1, new QTableWidgetItem(x));
    ui.boardTable->setItem(i, 2, new QTableWidgetItem(y));
    ui.boardTable->setItem(i, 3, new QTableWidgetItem(a));
    qInfo() << "Inserimento nuova board. Id:" << id << " Posizione:" << x << "." << y << " Potenza: " << a;
}

void SettingDialog::defaultValues() {
    QMessageBox::StandardButton reply;
    if (Utility::yesNoMessage(this, Strings::SET_DEF, Strings::SET_DEF_MSG)) {

        // Ripristino informazioni iniziali
        ui.portServerEdit->setValue(27015);
        ui.nEdit->setValue(3);
        ui.minEdit->setValue(1);
        ui.widthEdit->setValue(10);
        ui.heightEdit->setValue(10);
        ui.portEdit->setValue(27015);
        ui.hostEdit->setText("localhost");
        ui.dbEdit->setText("data");
        ui.portEdit->setValue(3306);
        ui.userEdit->setText("root");
        ui.passEdit->setText("NewRoot12Kz");
        ui.tableEdit->setText("stanza");
        ui.posTolEdit->setValue(2.0);
        ui.posPesoEdit->setValue(1);
        ui.timeTolEdit->setValue(3600);
        ui.timePesoEdit->setValue(1);
        ui.ssidPesoEdit->setValue(1);
        ui.secretEdit->setText("Progettopds1");
        this->fillBoardListDefault();
    } else {
        return;
    }
    qInfo() << Strings::SET_DEF_LOG;
}

bool SettingDialog::resetDB() {
    if (!this->isSettingValid()) return false;
    QSqlDatabase::removeDatabase(QSqlDatabase::database().connectionName());
    QSqlDatabase db{};
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(ui.hostEdit->text());
    db.setDatabaseName(ui.dbEdit->text());
    db.setPort(ui.portEdit->text().toInt());
    db.setUserName(ui.userEdit->text());
    db.setPassword(ui.passEdit->text());
    if (!db.open()) {
        Utility::warningMessage(Strings::ERR_DB, Strings::ERR_DB_MSG, db.lastError().text());
        return false;
    }
    QSqlQuery query{db};
    query.prepare(Query::DROP_TABLE_PACKET.arg(ui.tableEdit->text()));
    if (!query.exec()) {
        Utility::warningMessage(Strings::ERR_DB, Strings::ERR_DB_MSG, query.lastError().text());
        return false;
    }
    query.prepare(Query::DROP_TABLE_BOARD.arg(ui.tableEdit->text()));
    if (!query.exec()) {
        Utility::warningMessage(Strings::ERR_DB, Strings::ERR_DB_MSG, query.lastError().text());
        return false;
    }
    query.prepare(Query::CREATE_TABLE_PACKET.arg(ui.tableEdit->text()));
    if (!query.exec()) {
        Utility::warningMessage(Strings::ERR_DB, Strings::ERR_DB_MSG, query.lastError().text());
        return false;
    }

    query.prepare(Query::CREATE_TABLE_BOARD.arg(ui.tableEdit->text()));
    if (!query.exec()) {
        Utility::warningMessage(Strings::ERR_DB, Strings::ERR_DB_MSG, query.lastError().text());
        return false;
    }
    db.close();
    qInfo() << Strings::DB_RESET;
    Utility::infoMessage("Database Inizializzato", "Database Inizializzato correttamente");

    return true;
}

bool SettingDialog::isSettingValid() {
    QString err = "";
    int pos = 0;
    QRegExp port{"^()([1-9]|[1-5]?[0-9]{2,4}|6[1-4][0-9]{3}|65[1-4][0-9]{2}|655[1-2][0-9]|6553[1-5])$"};
    QRegExp host{
            R"(^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\-]*[a-zA-Z0-9])\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\-]*[A-Za-z0-9])$)"};
    QRegExp dbName{"^[0-9a-zA-Z$_]+$"};
    QRegExp username{"^[A-Za-z0-9_]{1,64}$"};

    /* STANZA */
    bool x;
    QRegExpValidator port_v{port, nullptr};
    QString port_s{ui.portServerEdit->text()};
    if (port_v.validate(port_s, pos) != QValidator::Acceptable) err += "Porta Server non valida.\n";
    if (ui.boardTable->rowCount() < 2) err += "Non sono state installate abbastanza schedine.\n";

    /* MONITORAGGIO */
    if (ui.secretEdit->text() == "") err += "Devi inserire un segreto.\n";

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

    /* DATABASE */
    QSqlDatabase::removeDatabase(QSqlDatabase::database().connectionName());
    QSqlDatabase db{};
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(ui.hostEdit->text());
    db.setDatabaseName(ui.dbEdit->text());
    db.setPort(ui.portEdit->text().toInt());
    db.setUserName(ui.userEdit->text());
    db.setPassword(ui.passEdit->text());
    if (!db.open()) {
        err += "Configurazione Database non valida.\n";
    } else {
        QSqlQuery query{db};
        query.prepare(Query::SELECT_ALL_PACKET.arg(ui.tableEdit->text()));
        if (!query.exec()) {
            err += "Tabella pacchetti non trovata.\n";
        }
        query.prepare(Query::SELECT_ALL_BOARD.arg(ui.tableEdit->text()));
        if (!query.exec()) {
            err += "Tabella schedine non trovata.\n";
        }
    }
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

void SettingDialog::compileValues() {
    // Riempimento dei vari campi grafici
    ui.nEdit->setValue(s.value("monitor/n").toInt());
    ui.minEdit->setValue(s.value("monitor/min").toInt());
    ui.widthEdit->setValue(s.value("room/width").toDouble());
    ui.heightEdit->setValue(s.value("room/height").toDouble());
    ui.portServerEdit->setValue(s.value("room/port").toInt());
    ui.hostEdit->setText(s.value("database/host").toString());
    ui.dbEdit->setText(s.value("database/name").toString());
    ui.portEdit->setValue(s.value("database/port").toInt());
    ui.userEdit->setText(s.value("database/user").toString());
    ui.passEdit->setText(s.value("database/pass").toString());
    ui.tableEdit->setText(s.value("database/table").toString());
    ui.retryEdit->setValue(s.value("Utility/RETRY_STEP_BOARD").toInt());
    ui.posTolEdit->setValue(s.value("mac/pos/tol").toDouble());
    ui.posPesoEdit->setValue(s.value("mac/pos/peso").toInt());
    ui.timeTolEdit->setValue(s.value("mac/time/tol").toInt());
    ui.timePesoEdit->setValue(s.value("mac/time/peso").toInt());
    ui.ssidPesoEdit->setValue(s.value("mac/ssid/peso").toInt());
    ui.secretEdit->setText(s.value("secret").toString());
    fillBoardList();
}

void
SettingDialog::addBoardToDB(const QString &id_board, const QString &pos_x, const QString &pos_y, const QString &a) {
    bool error = false;
    QSqlDatabase db = Utility::getDB(error);
    if (error) exit(-1);

    QSqlQuery query{db};
    query.prepare(Query::INSERT_BOARD.arg(ui.tableEdit->text()));
    query.bindValue(":id_board", id_board);
    query.bindValue(":pos_x", pos_x);
    query.bindValue(":pos_y", pos_y);
    query.bindValue(":a", a);
    if (!query.exec()) {
        Utility::warningMessage(Strings::ERR_DB, Strings::ERR_DB_MSG, query.lastError().text());
        return exit(-1);
    }
    db.close();
}
