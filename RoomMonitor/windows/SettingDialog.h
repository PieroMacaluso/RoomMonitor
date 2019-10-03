//
// Created by pieromack on 10/09/19.
//

#ifndef ROOMMONITOR_SETTINGDIALOG_H
#define ROOMMONITOR_SETTINGDIALOG_H


#include <QtWidgets/QDialog>
#include "ui_settings.h"
#include "ui_addboard.h"
#include "ui_modboard.h"

#include <QtCore/QSettings>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QtWidgets/QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlField>
#include <QtSql/QSqlDriver>
#include <QValidator>


class SettingDialog : public QDialog {

Q_OBJECT
public:
    /**
     * UI Finestra impostazioni
     */
    Ui::ConfigDialog ui{};

    /**
     * UI Dialog di aggiunta schedina
     */
    Ui::AddBoardDialog addBoardDialog{};

    /**
     * UI Dialog di modifica schedina
     */
    Ui::ModBoardDialog modBoardDialog{};

    /**
     * Impostazioni
     */
    QSettings s{"VALP", "RoomMonitoring"};

    /**
     * Lista delle schedine che viene popolata nel costruttore dopo averla estratta dalle impostazioni
     */
    QList<QStringList> boardList;

    /**
    * Costruttore di default che va a popolare tutte le impostazioni prelevandole da `QSettings`
    */
    SettingDialog();

    /**
     * Questa è una funzione statica utile per inizializzare il programma con le impostazioni predefinite nel caso
     * in cui queste non fossero ancora disponibili. Viene chiamato nel `main.cpp` all'avvio del programma.
     */
    static void settingCheckUp();

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
    void addBoard(const QString &id, const QString &x, const QString &y);

    /**
     * Funzione che che gestisce il segnale lanciato dalla view della tabella quando una casella viene modificata
     * all'interno della stessa.
     *
     * @param row
     * @param column
     */
    void elementChanged(int row, int column);

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
     * Controlla che i valori inseriti nelle caselle di testo della finestra di dialogo con cui si aggiungono
     * le schedine siano valori accettabili. La funzione è connessa alla `textChanged` delle LineEdit
     */
    void checkAddEdits();

    /**
     * Controlla che i valori inseriti nelle caselle di testo della finestra di dialogo con cui si modificano
     * le schedine siano valori accettabili. La funzione è connessa alla `textChanged` delle LineEdit
     */
    void checkModEdits();

    /**
     * Setup della finestra di dialogo di aggiunta schedine.
     */
    void setupAddBoard();

    /**
     * Setup della finestra di dialogo di modifica schedine.
     */
    void setupModBoard();

    /**
     * Routine di reset del sistema che viene chiamata premendo il pulsante applica dopo aver attivato la relativa
     * spunta nelle impostazioni del database.
     */
    void resetDB();

    /**Ripristina impostazioni iniziali**/
    void defaultValues();

    bool isSettingValid();
};


#endif //ROOMMONITOR_SETTINGDIALOG_H
