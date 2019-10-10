/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QCustomPlot *customPlot;
    QTableView *tableView;
    QCustomPlot *customPlot_2;
    QDateTimeEdit *From;
    QDateTimeEdit *To;
    QPushButton *Search;
    QPushButton *Reset;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QTableWidget *tableView_2;
    QFrame *frame;
    QPushButton *Search_2;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QPushButton *Aggior;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1588, 1171);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        customPlot = new QCustomPlot(centralWidget);
        customPlot->setObjectName(QString::fromUtf8("customPlot"));
        customPlot->setGeometry(QRect(10, 40, 441, 181));
        tableView = new QTableView(centralWidget);
        tableView->setObjectName(QString::fromUtf8("tableView"));
        tableView->setGeometry(QRect(450, 40, 851, 181));
        QPalette palette;
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush1(QColor(222, 222, 222, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush1);
        QBrush brush2(QColor(255, 255, 255, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Light, brush2);
        QBrush brush3(QColor(238, 238, 238, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Midlight, brush3);
        QBrush brush4(QColor(111, 111, 111, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Dark, brush4);
        QBrush brush5(QColor(148, 148, 148, 255));
        brush5.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Active, QPalette::Text, brush);
        palette.setBrush(QPalette::Active, QPalette::BrightText, brush2);
        palette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Active, QPalette::Base, brush2);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Active, QPalette::AlternateBase, brush3);
        QBrush brush6(QColor(255, 255, 220, 255));
        brush6.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::ToolTipBase, brush6);
        palette.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Light, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::Midlight, brush3);
        palette.setBrush(QPalette::Inactive, QPalette::Dark, brush4);
        palette.setBrush(QPalette::Inactive, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette.setBrush(QPalette::Inactive, QPalette::BrightText, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush3);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush6);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Light, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Midlight, brush3);
        palette.setBrush(QPalette::Disabled, QPalette::Dark, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::BrightText, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush6);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        tableView->setPalette(palette);
        tableView->setAlternatingRowColors(true);
        customPlot_2 = new QCustomPlot(centralWidget);
        customPlot_2->setObjectName(QString::fromUtf8("customPlot_2"));
        customPlot_2->setEnabled(false);
        customPlot_2->setGeometry(QRect(10, 400, 431, 251));
        From = new QDateTimeEdit(centralWidget);
        From->setObjectName(QString::fromUtf8("From"));
        From->setEnabled(true);
        From->setGeometry(QRect(330, 300, 194, 22));
        From->setAcceptDrops(true);
        From->setAutoFillBackground(true);
        From->setButtonSymbols(QAbstractSpinBox::PlusMinus);
        From->setCalendarPopup(true);
        To = new QDateTimeEdit(centralWidget);
        To->setObjectName(QString::fromUtf8("To"));
        To->setGeometry(QRect(330, 330, 194, 22));
        To->setAcceptDrops(true);
        To->setButtonSymbols(QAbstractSpinBox::PlusMinus);
        To->setCalendarPopup(true);
        Search = new QPushButton(centralWidget);
        Search->setObjectName(QString::fromUtf8("Search"));
        Search->setGeometry(QRect(10, 360, 75, 23));
        QFont font;
        font.setFamily(QString::fromUtf8("Source Code Pro"));
        font.setPointSize(10);
        font.setBold(true);
        font.setWeight(75);
        Search->setFont(font);
        Reset = new QPushButton(centralWidget);
        Reset->setObjectName(QString::fromUtf8("Reset"));
        Reset->setGeometry(QRect(450, 360, 75, 23));
        Reset->setFont(font);
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 230, 511, 51));
        label->setFont(font);
        label->setAutoFillBackground(false);
        label->setFrameShape(QFrame::StyledPanel);
        label->setFrameShadow(QFrame::Plain);
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 300, 151, 16));
        label_2->setFont(font);
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 330, 131, 16));
        label_3->setFont(font);
        tableView_2 = new QTableWidget(centralWidget);
        tableView_2->setObjectName(QString::fromUtf8("tableView_2"));
        tableView_2->setGeometry(QRect(440, 460, 861, 192));
        tableView_2->setAlternatingRowColors(true);
        tableView_2->setSortingEnabled(false);
        frame = new QFrame(centralWidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(560, 270, 741, 121));
        frame->setFrameShape(QFrame::Box);
        frame->setFrameShadow(QFrame::Raised);
        Search_2 = new QPushButton(frame);
        Search_2->setObjectName(QString::fromUtf8("Search_2"));
        Search_2->setGeometry(QRect(370, 80, 361, 31));
        Search_2->setFont(font);
        label_4 = new QLabel(frame);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(10, 0, 711, 51));
        label_4->setFont(font);
        label_4->setFrameShape(QFrame::NoFrame);
        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(450, 10, 851, 31));
        label_5->setFont(font);
        label_5->setAutoFillBackground(false);
        label_5->setFrameShape(QFrame::StyledPanel);
        label_5->setFrameShadow(QFrame::Plain);
        label_6 = new QLabel(centralWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(10, 10, 441, 31));
        label_6->setFont(font);
        label_6->setAutoFillBackground(false);
        label_6->setFrameShape(QFrame::StyledPanel);
        label_6->setFrameShadow(QFrame::Plain);
        label_7 = new QLabel(centralWidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(440, 420, 861, 41));
        label_7->setFont(font);
        label_7->setAutoFillBackground(false);
        label_7->setFrameShape(QFrame::StyledPanel);
        label_7->setFrameShadow(QFrame::Plain);
        Aggior = new QPushButton(centralWidget);
        Aggior->setObjectName(QString::fromUtf8("Aggior"));
        Aggior->setGeometry(QRect(990, 230, 311, 31));
        Aggior->setFont(font);
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1588, 21));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        Search->setText(QCoreApplication::translate("MainWindow", "Search", nullptr));
        Reset->setText(QCoreApplication::translate("MainWindow", "Reset", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Selezionare il range per visionare i mac rilevati\n"
"Il valore minimo da cui partire \303\250 quello del primo mac trovato\n"
"mentre il valore massimo del range sono la data e ora attuali", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Data inizio range", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Data fine range", nullptr));
        Search_2->setText(QCoreApplication::translate("MainWindow", "Visualizza il grafico in una nuova finestra", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Selezionare il range per visionare la successione delle posizioni dei vari mac rilevati in tale range \n"
"nell' area di riferimento. Il valore minimo da cui partire \303\250 quello del primo mac trovato mentre\n"
"il valore massimo del range sono la data e ora attuali", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "Dati attualmente presenti nel database(altri dati in fase di acquisizione)", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "Numero rilevazioni suddivise per mac dei dati presenti", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "Range minimo e massimo in cui \303\250 stato rilevato il mac (solo all'interno del range temporale selezionato)", nullptr));
        Aggior->setText(QCoreApplication::translate("MainWindow", "Aggiornamento Dati", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
