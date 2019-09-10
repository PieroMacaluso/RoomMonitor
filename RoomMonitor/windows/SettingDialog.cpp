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
}

void SettingDialog::settingCheckUp() {
    // Impostazioni iniziali, se non sono mai state configurate
    /* TODO: verificare che possano essere spostate nel programma principale per funzionare di base senza per forza
     * aprire le impostazioni
     */
    if (s.value("monitor/A").isNull()) s.setValue("monitor/A", 3);
    if (s.value("monitor/n").isNull()) s.setValue("monitor/n", -55);
    if (s.value("room/width").isNull()) s.setValue("room/width", 10);
    if (s.value("room/height").isNull()) s.setValue("room/height", 10);

    // TODO: gestione vettore con schedine e relative posizioni
}

void SettingDialog::setupConnect() {
    connect(ui.buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &SettingDialog::apply);
}
