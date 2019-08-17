#include "princ.h"
#include "ui_princ.h"
#include "mainwindow.h"
Princ::Princ(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Princ)
{
    ui->setupUi(this);
}

Princ::~Princ()
{
    delete ui;
}

void Princ::on_Next_clicked()
{
    this->close();
    Princ Finestra;
    ui->label->setAlignment(Qt::AlignCenter);
   this->setStyleSheet("{background-image:url(\"bkg.jpg\"); background-position: center;}");
    //this->showMaximized();

    /* QPixmap bkgnd("/home/user/Pictures/background.png");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);*/
}
