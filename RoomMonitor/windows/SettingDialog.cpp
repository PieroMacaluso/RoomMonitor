//
// Created by pieromack on 10/09/19.
//

#include "SettingDialog.h"
#include "../monitoring/Board.h"

SettingDialog::SettingDialog() {
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
}

void SettingDialog::settingCheckUp() {
    QSettings su{};
    // Impostazioni iniziali, se non sono mai state configurate
    /* TODO: verificare che possano essere spostate nel programma principale per funzionare di base senza per forza
     * aprire le impostazioni
     */
    if (su.value("monitor/A").isNull()) su.setValue("monitor/A", 3);
    if (su.value("monitor/n").isNull()) su.setValue("monitor/n", -55);
    if (su.value("room/width").isNull()) su.setValue("room/width", 10);
    if (su.value("room/height").isNull()) su.setValue("room/height", 10);
    if (su.value("room/port").isNull()) su.setValue("room/port", 27015);
    if (su.value("database/host").isNull()) su.setValue("database/host", "localhost");
    if (su.value("database/name").isNull()) su.setValue("database/name", "data");
    if (su.value("database/port").isNull()) su.setValue("database/port", 3306);
    if (su.value("database/user").isNull()) su.setValue("database/user", "root");
    if (su.value("database/pass").isNull()) su.setValue("database/pass", "NewRoot12Kz");


    // TODO: gestione vettore con schedine e relative posizioni
}

void SettingDialog::setupConnect() {
    connect(ui.buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &SettingDialog::apply);
}
