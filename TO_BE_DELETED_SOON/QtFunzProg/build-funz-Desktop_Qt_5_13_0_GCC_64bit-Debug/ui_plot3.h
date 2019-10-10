/********************************************************************************
** Form generated from reading UI file 'plot3.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLOT3_H
#define UI_PLOT3_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_Plot3
{
public:
    QComboBox *comboBox;
    QPushButton *pushButton;
    QCustomPlot *customPlot_3;
    QDateTimeEdit *To_2;
    QDateTimeEdit *From_2;
    QPushButton *Search_2;
    QLabel *label_6;
    QLabel *label_5;
    QPushButton *Reset_2;
    QLabel *label_2;
    QLabel *label_4;

    void setupUi(QDialog *Plot3)
    {
        if (Plot3->objectName().isEmpty())
            Plot3->setObjectName(QString::fromUtf8("Plot3"));
        Plot3->resize(1109, 779);
        comboBox = new QComboBox(Plot3);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));
        comboBox->setGeometry(QRect(30, 290, 181, 22));
        pushButton = new QPushButton(Plot3);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(240, 290, 181, 23));
        QFont font;
        font.setFamily(QString::fromUtf8("Source Code Pro"));
        font.setPointSize(10);
        font.setBold(true);
        font.setWeight(75);
        pushButton->setFont(font);
        customPlot_3 = new QCustomPlot(Plot3);
        customPlot_3->setObjectName(QString::fromUtf8("customPlot_3"));
        customPlot_3->setEnabled(false);
        customPlot_3->setGeometry(QRect(30, 340, 791, 331));
        To_2 = new QDateTimeEdit(Plot3);
        To_2->setObjectName(QString::fromUtf8("To_2"));
        To_2->setGeometry(QRect(230, 130, 194, 22));
        To_2->setAcceptDrops(true);
        To_2->setButtonSymbols(QAbstractSpinBox::PlusMinus);
        To_2->setCalendarPopup(true);
        From_2 = new QDateTimeEdit(Plot3);
        From_2->setObjectName(QString::fromUtf8("From_2"));
        From_2->setEnabled(true);
        From_2->setGeometry(QRect(230, 90, 194, 22));
        From_2->setAcceptDrops(true);
        From_2->setButtonSymbols(QAbstractSpinBox::PlusMinus);
        From_2->setCalendarPopup(true);
        Search_2 = new QPushButton(Plot3);
        Search_2->setObjectName(QString::fromUtf8("Search_2"));
        Search_2->setGeometry(QRect(30, 180, 75, 23));
        Search_2->setFont(font);
        label_6 = new QLabel(Plot3);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(30, 130, 131, 16));
        label_6->setFont(font);
        label_5 = new QLabel(Plot3);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(30, 90, 151, 16));
        label_5->setFont(font);
        Reset_2 = new QPushButton(Plot3);
        Reset_2->setObjectName(QString::fromUtf8("Reset_2"));
        Reset_2->setGeometry(QRect(350, 180, 75, 23));
        Reset_2->setFont(font);
        label_2 = new QLabel(Plot3);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(30, 220, 1031, 51));
        label_2->setFont(font);
        label_2->setFrameShape(QFrame::Panel);
        label_4 = new QLabel(Plot3);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(30, 10, 1031, 61));
        label_4->setFont(font);
        label_4->setFrameShape(QFrame::StyledPanel);

        retranslateUi(Plot3);

        QMetaObject::connectSlotsByName(Plot3);
    } // setupUi

    void retranslateUi(QDialog *Plot3)
    {
        Plot3->setWindowTitle(QCoreApplication::translate("Plot3", "Terzo Grafico", nullptr));
        pushButton->setText(QCoreApplication::translate("Plot3", "Visualizza il grafico", nullptr));
        Search_2->setText(QCoreApplication::translate("Plot3", "Search", nullptr));
        label_6->setText(QCoreApplication::translate("Plot3", "Data fine range", nullptr));
        label_5->setText(QCoreApplication::translate("Plot3", "Data inizio range", nullptr));
        Reset_2->setText(QCoreApplication::translate("Plot3", "Reset", nullptr));
        label_2->setText(QCoreApplication::translate("Plot3", "Selezionare l'indirizzo mac del quale si desidera visualizzarne  la successione delle posizioni all\342\200\231interno dell\342\200\231area.\n"
"(i mac presenti sono solo quelli rilevati nell'intervallo precedentemente selezionato)", nullptr));
        label_4->setText(QCoreApplication::translate("Plot3", "Selezionare il range per visionare la successione delle posizioni dei vari mac rilevati in tale range nella area di riferimento. \n"
"Il valore minimo da cui partire \303\250 quello del primo mac trovato mentre il valore massimo del range \303\250 la data e ora attuali.", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Plot3: public Ui_Plot3 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLOT3_H
