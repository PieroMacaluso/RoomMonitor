//
// Created by pieromack on 10/09/19.
//

#include "SettingDialog.h"

SettingDialog::SettingDialog() {
    // Linea che serve per poter salvare nelle impostazioni una QList<QStringList>
    qRegisterMetaTypeStreamOperators<QList<QStringList>>("Stuff");
    // Inizializzazione SettingDialog con impostazioni da QSettings.
    settingCheckUp();
    ui.setupUi(this);
    setupConnect();
    initializeBoardList();
    if (SettingDialog::settingCheckUp()) this->compileValues();
    else {
        ui.resetCheck->setChecked(true);
        ui.resetCheck->setDisabled(true);
    }
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

    connect(ui.secretEcho, &QToolButton::pressed, [&](){
        ui.secretEdit->setEchoMode(QLineEdit::Normal);
    });

    connect(ui.secretEcho, &QToolButton::released, [&](){
        ui.secretEdit->setEchoMode(QLineEdit::Password);
    });


    connect(ui.passEcho, &QToolButton::pressed, [&](){
        ui.passEdit->setEchoMode(QLineEdit::Normal);
    });

    connect(ui.passEcho, &QToolButton::released, [&](){
        ui.passEdit->setEchoMode(QLineEdit::Password);
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
    add.setModal(true);
    setupAddBoard();

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
            addBoard(addBoardDialog.idEdit->text(), addBoardDialog.xEdit->text(), addBoardDialog.yEdit->text(),
                     addBoardDialog.aEdit->text());
            break;
        }
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
    modBoardDialog.aEdit->setText(list[3]);

    setupModBoard();
    while (add.exec()) {
        if (modBoardDialog.idEdit->text().isEmpty() || modBoardDialog.xEdit->text().isEmpty() ||
            modBoardDialog.yEdit->text().isEmpty() || modBoardDialog.aEdit->text().isEmpty()) {
            QMessageBox msgBox;
            msgBox.setStandardButtons(QMessageBox::Close);
            msgBox.setWindowTitle("Errore inserimento dati schedina");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setText("Inserimento non valido, riprovare!");
            continue;
        }
        bool valid = true;
        for (int i = 0; i < ui.boardTable->rowCount(); i++) {
            if (modBoardDialog.idEdit->text() == ui.boardTable->item(i, 0)->text() &&
                modBoardDialog.idEdit->text() != list[0]) {
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
            ui.boardTable->setItem(i, 0, new QTableWidgetItem(modBoardDialog.idEdit->text()));
            ui.boardTable->setItem(i, 1, new QTableWidgetItem(modBoardDialog.xEdit->text()));
            ui.boardTable->setItem(i, 2, new QTableWidgetItem(modBoardDialog.yEdit->text()));
            ui.boardTable->setItem(i, 3, new QTableWidgetItem(modBoardDialog.aEdit->text()));
            break;
        }
    }
}

void SettingDialog::apply() {
    if (!isSettingValid()) return;
    if (ui.resetCheck->isChecked()) {
        if (!resetDB()) return;
    }
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
    s.setValue("Utility/RETRY_STEP_BOARD", ui.retryEdit->text().toInt());
    s.setValue("mac/pos/tol", ui.posTolEdit->text().toInt());
    s.setValue("mac/pos/peso", ui.posPesoEdit->text().toInt());
    s.setValue("mac/pos/check", ui.posCheck->isChecked());
    s.setValue("mac/time/tol", ui.timeTolEdit->text().toInt());
    s.setValue("mac/time/peso", ui.timePesoEdit->text().toInt());
    s.setValue("mac/time/check", ui.timeCheck->isChecked());
    s.setValue("mac/ssid/peso", ui.ssidPesoEdit->text().toInt());
    s.setValue("mac/ssid/check", ui.ssidCheck->isChecked());
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

void SettingDialog::setupAddBoard() {
    addBoardDialog.idEdit->setValidator(new QIntValidator());
    addBoardDialog.xEdit->setValidator(new QDoubleValidator());
    addBoardDialog.yEdit->setValidator(new QDoubleValidator());
    addBoardDialog.aEdit->setValidator(new QIntValidator());
    checkAddEdits();
    connect(addBoardDialog.idEdit, &QLineEdit::textChanged, this, &SettingDialog::checkAddEdits);
    connect(addBoardDialog.xEdit, &QLineEdit::textChanged, this, &SettingDialog::checkAddEdits);
    connect(addBoardDialog.yEdit, &QLineEdit::textChanged, this, &SettingDialog::checkAddEdits);
    connect(addBoardDialog.aEdit, &QLineEdit::textChanged, this, &SettingDialog::checkAddEdits);
}

void SettingDialog::checkAddEdits() {
    bool id;
    bool x;
    bool y;
    bool a;
    addBoardDialog.idEdit->text().toInt(&id);
    addBoardDialog.xEdit->text().toDouble(&x);
    addBoardDialog.yEdit->text().toDouble(&y);
    addBoardDialog.aEdit->text().toInt(&a);
    addBoardDialog.buttonBox->button(QDialogButtonBox::Ok)->setDisabled(
            !(id && x && y && addBoardDialog.aEdit->text().toInt(&a) < 0));
}


void SettingDialog::setupModBoard() {
    modBoardDialog.idEdit->setValidator(new QIntValidator());
    modBoardDialog.xEdit->setValidator(new QDoubleValidator());
    modBoardDialog.yEdit->setValidator(new QDoubleValidator());
    modBoardDialog.aEdit->setValidator(new QIntValidator());
    checkModEdits();
    connect(modBoardDialog.idEdit, &QLineEdit::textChanged, this, &SettingDialog::checkModEdits);
    connect(modBoardDialog.xEdit, &QLineEdit::textChanged, this, &SettingDialog::checkModEdits);
    connect(modBoardDialog.yEdit, &QLineEdit::textChanged, this, &SettingDialog::checkModEdits);
    connect(modBoardDialog.aEdit, &QLineEdit::textChanged, this, &SettingDialog::checkModEdits);

}

void SettingDialog::checkModEdits() {
    bool id;
    bool x;
    bool y;
    bool a;
    modBoardDialog.idEdit->text().toInt(&id);
    modBoardDialog.xEdit->text().toDouble(&x);
    modBoardDialog.yEdit->text().toDouble(&y);
    modBoardDialog.aEdit->text().toInt(&a);
    modBoardDialog.buttonBox->button(QDialogButtonBox::Ok)->setDisabled(
            !(id && x && y && a && modBoardDialog.aEdit->text().toInt(&a) < 0));
}

void SettingDialog::defaultValues() {
    QMessageBox::StandardButton reply;
    if (Utility::yesNoMessage(this, Strings::SET_DEF, Strings::SET_DEF_MSG)) {

        // Ripristino informazioni iniziali
        ui.portServerEdit->setText(QString::number(27015));
        ui.nEdit->setText(QString::number(3));
        ui.minEdit->setValue(1);
        ui.widthEdit->setText(QString::number(10));
        ui.heightEdit->setText(QString::number(10));
        ui.portEdit->setText(QString::number(27015));
        ui.hostEdit->setText("localhost");
        ui.dbEdit->setText("data");
        ui.portEdit->setText(QString::number(3306));
        ui.userEdit->setText("root");
        ui.passEdit->setText("NewRoot12Kz");
        ui.tableEdit->setText("stanza");
        ui.posTolEdit->setText(QString::number(0.5));
        ui.posPesoEdit->setText(QString::number(1));
        ui.posCheck->setChecked(true);
        ui.timeTolEdit->setText(QString::number(3600));
        ui.timePesoEdit->setText(QString::number(1));
        ui.timeCheck->setChecked(true);
        ui.ssidPesoEdit->setText(QString::number(1));
        ui.ssidCheck->setChecked(true);
        this->fillBoardListDefault();
    } else {
        return;
    }
    qInfo() << Strings::SET_DEF_LOG;
}

bool SettingDialog::resetDB() {
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
    return true;
}

bool SettingDialog::isSettingValid() {
    // TODO: Controllare e testare validazione Input Impostazioni
    QString err = "";
    int pos = 0;
    QRegExp port{"^()([1-9]|[1-5]?[0-9]{2,4}|6[1-4][0-9]{3}|65[1-4][0-9]{2}|655[1-2][0-9]|6553[1-5])$"};
    QRegExp host{
            R"(^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\-]*[a-zA-Z0-9])\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9\-]*[A-Za-z0-9])$)"};
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
    ui.retryEdit->setValue(s.value("Utility/RETRY_STEP_BOARD").toInt());
    ui.posTolEdit->setText(s.value("mac/pos/tol").toString());
    ui.posPesoEdit->setText(s.value("mac/pos/peso").toString());
    ui.posCheck->setChecked(s.value("mac/pos/check").toBool());
    ui.timeTolEdit->setText(s.value("mac/time/tol").toString());
    ui.timePesoEdit->setText(s.value("mac/time/peso").toString());
    ui.timeCheck->setChecked(s.value("mac/time/check").toBool());
    ui.ssidPesoEdit->setText(s.value("mac/ssid/peso").toString());
    ui.ssidCheck->setChecked(s.value("mac/ssid/check").toBool());
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
