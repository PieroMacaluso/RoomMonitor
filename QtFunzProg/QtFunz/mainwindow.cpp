#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

#include<QSqlError>
#include<QSqlTableModel>
#include <QSqlQuery>

#include <QtSql>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    MainWindow::makePlot();
    nDatabase=QSqlDatabase::addDatabase("QMYSQL");
    nDatabase.setHostName("localhost");
    nDatabase.setDatabaseName("data");
    nDatabase.setPort(3306);
    nDatabase.setUserName("root");
    nDatabase.setPassword("NewRoot12Kz");
    if(!nDatabase.open()){
        QMessageBox::critical(this, "Error", nDatabase.lastError().text());
        return;
    }
    nModel=new QSqlTableModel(this);
    nModel ->setTable("campi");
    nModel->select();
    ui->tableView->setModel(nModel);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::makePlot(){

    // set dark background gradient:
    QLinearGradient gradient(0, 0, 0, 400);
    gradient.setColorAt(0, QColor(90, 90, 90));
    gradient.setColorAt(0.38, QColor(105, 105, 105));
    gradient.setColorAt(1, QColor(70, 70, 70));
    ui->customPlot->setBackground(QBrush(gradient));

    // create empty bar chart objects:

    QCPBars *mac = new QCPBars(ui->customPlot->xAxis, ui->customPlot->yAxis);
    mac->setAntialiased(false);
    mac->setStackingGap(1);
    // set names and colors:
    mac->setName("Indirizzi MAC rilevati");
    mac->setPen(QPen(QColor(111, 9, 176).lighter(170)));
    mac->setBrush(QColor(111, 9, 176));

    // prepare x axis with country labels:
    QVector<double> ticks;
    QVector<QString> labels;
    ticks << 1 << 2 << 3 << 4 << 5 << 6 << 7;

    QSqlQuery comandoSql;
    comandoSql.exec("select * from campi");

    //consumiamo un record alla volta il recordset restituito restituisce boolean
    while(comandoSql.next()){

        comandoSql.value("mac").toString();//oppure .value(0)..1...2...

    }

    labels << "MAC1" << "MAC2" << "MAC3" << "MAC4" << "MAC5" << "MAC6" << "MAC7";
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);
    ui->customPlot->xAxis->setTicker(textTicker);
    ui->customPlot->xAxis->setTickLabelRotation(60);
    ui->customPlot->xAxis->setSubTicks(false);
    ui->customPlot->xAxis->setTickLength(0, 4);
    ui->customPlot->xAxis->setRange(0, 8);
    ui->customPlot->xAxis->setBasePen(QPen(Qt::white));
    ui->customPlot->xAxis->setTickPen(QPen(Qt::white));
    ui->customPlot->xAxis->grid()->setVisible(true);
    ui->customPlot->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
    ui->customPlot->xAxis->setTickLabelColor(Qt::white);
    ui->customPlot->xAxis->setLabelColor(Qt::white);

    // prepare y axis:
    ui->customPlot->yAxis->setRange(0, 12.1);
    ui->customPlot->yAxis->setPadding(5); // a bit more space to the left border
    ui->customPlot->yAxis->setLabel("Descrizione grafico");
    ui->customPlot->yAxis->setBasePen(QPen(Qt::white));
    ui->customPlot->yAxis->setTickPen(QPen(Qt::white));
    ui->customPlot->yAxis->setSubTickPen(QPen(Qt::white));
    ui->customPlot->yAxis->grid()->setSubGridVisible(true);
    ui->customPlot->yAxis->setTickLabelColor(Qt::white);
    ui->customPlot->yAxis->setLabelColor(Qt::white);
    ui->customPlot->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
    ui->customPlot->yAxis->grid()->setSubGridPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));

    // Add data:
    QVector<double> macData;//, nuclearData, regenData;
    macData  << 0.86*10.5 << 0.83*5.5 << 0.84*5.5 << 0.52*5.8 << 0.89*5.2 << 0.90*4.2 << 0.67*11.2;

    mac->setData(ticks, macData);

    // setup legend:
    ui->customPlot->legend->setVisible(true);
    ui->customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignHCenter);
    ui->customPlot->legend->setBrush(QColor(255, 255, 255, 100));
    ui->customPlot->legend->setBorderPen(Qt::NoPen);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    ui->customPlot->legend->setFont(legendFont);
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);



}
