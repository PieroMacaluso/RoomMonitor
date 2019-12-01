//
// Created by pieromack on 10/09/19.
//

#ifndef ROOMMONITOR_SETTINGDIALOG_H
#define ROOMMONITOR_SETTINGDIALOG_H


#include <QtWidgets/QDialog>
#include "ui_settings.h"
#include "ui_addboard.h"
#include <QtWidgets/QTableWidgetItem>
#include "../monitoring/Board.h"
#include <QtCore/QSettings>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QtWidgets/QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlField>
#include <QtSql/QSqlDriver>
#include <QValidator>
#include <Query.h>
#include <Utility.h>



class SettingDialog : public QDialog {

Q_OBJECT
public:
    /**
     * UI Finestra impostazioni
     */
    Ui::ConfigDialog ui{};

    /**
     * UI Dialog di aggiunta/modifica schedina
     */
    Ui::AddBoardDialog addBoardDialog{};

    /**
     * Impostazioni
     */
    QSettings s{Utility::ORGANIZATION, Utility::APPLICATION};

    /**
    * Costruttore di default che va a popolare tutte le impostazioni prelevandole da `QSettings`
    */
    SettingDialog();

    /**
     * Questa è una funzione statica utile per inizializzare il programma con le impostazioni predefinite nel caso
     * in cui queste non fossero ancora disponibili. Viene chiamato nel `main.cpp` all'avvio del programma.
     */
    static bool settingCheckUp();

    /**
     * Funzione di inizializzazione di tutte le connect utili della finestra SettingDialog
     */
    void setupConnect();

public slots:

    /**
     * Slot richiamato dal pulsante applica, permette di applicare tutte le impostazioni presenti nella finestra
     * delle impostazioni.
     */
    void apply();

    /**
     * Funzione di inizializzazione della tabella contenente l'elenco delle schedine e le relative posizioni
     */
    void initializeBoardList();

    /**
     * Funzione che aggiunge una schedina all'elenco delle impostazioni.
     * @param id    id della schedina
     * @param x     coordinata x della schedina
     * @param y     coordinata y della schedina
     */
    void addBoard(const QString &id, const QString &x, const QString &y, const QString &a);

    /**
     * Rimuove la schedina che è selezionata al momento. Viene triggerata dal pulsante `removeBoard`
     */
    void removeSelected();

    /**
     * Funzione che apre una finestra di dialogo per procedere all'inserimento di una nuova schedina.
     * Viene triggerato dal click sul relativo pulsante.
     *
     */
    void openDialogAdd();

    /**
     * Funzione che apre una finestra di dialogo per procedere alla modifica dei dati di una schedina.
     * Viene triggerato dal click sul relativo pulsante.
     */
    void openDialogMod();

    /**
     * Routine di reset del sistema che viene chiamata premendo il pulsante applica dopo aver attivato la relativa
     * spunta nelle impostazioni del database.
     */
    bool resetDB();

    /**
     * Funzione che ripristina le impostazioni di default
    */
    void defaultValues();

    /**
     * Controllo e validazione delle impostazioni inserite
    */
    bool isSettingValid();

    /**
     * Compila i valori dei vari form, con i dati delle impostazioni salvate.
     */
    void compileValues();

    /**
     * Aggiunge la schedina al Database
     * @param id_board      id schedina
     * @param pos_x         posizione x
     * @param pos_y         posizione y
     * @param a
     */
    void addBoardToDB(const QString& id_board, const QString& pos_x, const QString& pos_y, const QString& a);

    /**
     * Riempimento della tabella delle schedine con le schedine da database
     */
    void fillBoardList();

    /**
     * Riempimento della tabella delle schedine con le schedine da impostazioni default
     */
    void fillBoardListDefault();

    /**
     * Ripristina schedine da Database
     */
    void restoreBoards();
};


#endif //ROOMMONITOR_SETTINGDIALOG_H
