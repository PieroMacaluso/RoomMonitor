/********************************************************************************
** Form generated from reading UI file 'princ.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PRINC_H
#define UI_PRINC_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Princ
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *Next;

    void setupUi(QDialog *Princ)
    {
        if (Princ->objectName().isEmpty())
            Princ->setObjectName(QString::fromUtf8("Princ"));
        Princ->resize(590, 394);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Princ->sizePolicy().hasHeightForWidth());
        Princ->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(Princ);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label = new QLabel(Princ);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy1);
        QFont font;
        font.setFamily(QString::fromUtf8("Freestyle Script"));
        font.setPointSize(24);
        label->setFont(font);
        label->setTextFormat(Qt::AutoText);
        label->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        Next = new QPushButton(Princ);
        Next->setObjectName(QString::fromUtf8("Next"));
        Next->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout->addWidget(Next);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(Princ);

        QMetaObject::connectSlotsByName(Princ);
    } // setupUi

    void retranslateUi(QDialog *Princ)
    {
        Princ->setWindowTitle(QCoreApplication::translate("Princ", "PDS 2017-2018", nullptr));
        label->setText(QCoreApplication::translate("Princ", "Progetto realizzato da:\n"
"s252894 Macaluso Piero\n"
"s255279 Manicone Lorenzo\n"
"s255346 Rosso Valentino\n"
"s255270 Turco Angelo", nullptr));
        Next->setText(QCoreApplication::translate("Princ", "NEXT", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Princ: public Ui_Princ {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PRINC_H
