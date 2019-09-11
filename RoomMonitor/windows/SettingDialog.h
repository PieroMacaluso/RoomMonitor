//
// Created by pieromack on 10/09/19.
//

#ifndef ROOMMONITOR_SETTINGDIALOG_H
#define ROOMMONITOR_SETTINGDIALOG_H


#include <QtWidgets/QDialog>
#include "../ui_settings.h"
#include <QtCore/QSettings>


class SettingDialog : public QDialog {
Q_OBJECT
public:
    Ui::ConfigDialog ui{};
    QSettings s{};

    SettingDialog();

    /**
     * Questa è una funzione statica utile per inizializzare il programma con le impostazioni predefinite nel caso
     * in cui queste non fossero ancora disponibili.
     */
    static void settingCheckUp();

    /**
     * Funzione di inizializzazione di tutte le connect utili della finestra.
     */
    void setupConnect();

public slots:
    /**
     * Slot richiamato dal pulsante applica, permette di applicare tutte le impostazioni presenti nella finestra
     * delle impostazioni.
     */
    void apply() {
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

        this->close();
    }

};


#endif //ROOMMONITOR_SETTINGDIALOG_H
