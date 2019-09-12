//
// Created by pieromack on 10/09/19.
//

#ifndef ROOMMONITOR_SETTINGDIALOG_H
#define ROOMMONITOR_SETTINGDIALOG_H


#include <QtWidgets/QDialog>
#include "../ui_settings.h"
#include "../ui_addboard.h"
#include <QtCore/QSettings>


class SettingDialog : public QDialog {
Q_OBJECT
public:
    Ui::ConfigDialog ui{};
    Ui::AddBoardDialog addBoardDialog{};
    QSettings s{};
    QList<QStringList> boardList;


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
    void apply();

    void initializeBoardList();

    void addBoard(const QString& id, const QString& x, const QString& y);
    void elementChanged(int row, int column);
    void removeSelected();
    void openDialogAdd();

};


#endif //ROOMMONITOR_SETTINGDIALOG_H
