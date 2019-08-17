#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "princ.h"
#include <QMessageBox>
#include "plot3.h"
#include<QSqlError>
#include<QSqlTableModel>
#include <QSqlQuery>
#include <QtDataVisualization/Q3DScene>

#include <QtSql>
/*ReturnType ClassName::nameOfFunction (ParameterType parameter){
// What the function should do
}*/
void MainWindow::connessione(){//funzione di connessione al DB
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
void MainWindow::resetRangeSelect(){//funzione di reset del range selezionati nel QDateTime Edit
   //IMPOSTAZIONE DATA MINIMA E MASSIMA Data/Time edit(In base ai dati presenti nel DB)
    QSqlQuery *qryData=new QSqlQuery(nDatabase);
    qryData->prepare("select timestamp from campi group by mac");
    qryData->exec();
    QDateTime maxData(QDate(0000, 1, 1), QTime(0,0,0));// da impostare valore MINIMO
    QDateTime minData= QDateTime::currentDateTime();


    while (qryData->next()) {

           QDateTime data = qryData->value(0).toDateTime();

           if(data>maxData)

            maxData=data;

           if(data<minData)
            minData=data;

        // qDebug()<<data;
       }
    ui->From->setMaximumDateTime(maxData);
    ui->From->setMinimumDateTime(minData);

    ui->To->setMinimumDateTime(minData);
    ui->To->setMaximumDateTime(QDateTime::currentDateTime());// da vedere se metterlo veramente come vincolo

    ui->From->setDateTime(minData);
    ui->To->setDateTime(minData);


}
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //Pagina introduttiva
    Princ finestra;
    finestra.setModal(true);
    finestra.exec();
    //
    this->showMaximized();

    ui->setupUi(this);
    MainWindow::makePlot();
    ui->customPlot_2->setVisible(false);
    ui->tableView_2->setVisible(false);
    ui->label_7->setVisible(false);

    ui->tableView_2->setColumnCount(3);
    auto header = QStringList() << "MAC" << "Prima occorrenza trovata"<< "Ultima occorrenza trovata";
    ui->tableView_2->setHorizontalHeaderLabels(header);
    ui->tableView_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connessione();

    resetRangeSelect();


   // connect(ui->customPlot_2, SIGNAL(Mouse_Pressed()), this, SLOT(Mouse_Pressed()));per gestione segnali eventi

}

MainWindow::~MainWindow()
{
    delete ui;
}
/*void MainWindow::Mouse_Pressed(){//per gestione segnali eventi
    qDebug() << "Clicked: ";

}*/

void MainWindow::makePlot(){


    // set dark background gradient:
    QLinearGradient gradient(0, 0, 0, 400);
    gradient.setColorAt(0, QColor(90, 90, 90));
    gradient.setColorAt(0.38, QColor(105, 105, 105));
    gradient.setColorAt(1, QColor(70, 70, 70));
    ui->customPlot->setBackground(QBrush(gradient));

    int i=0;// Contatore del numero di Mac distinti
    int max=-1;// valore necessario per settare il range asse y(probabilmente esiste anche funzione automatica)
    QVector<double> macData;//, Vettore con VALORE ALTEZZA BARRE
    // create empty bar chart objects:

    QCPBars *mac = new QCPBars(ui->customPlot->xAxis, ui->customPlot->yAxis);
    mac->setAntialiased(false);
    mac->setStackingGap(1);
    // set names and colors:
    mac->setName("Indirizzi MAC rilevati");
    mac->setPen(QPen(QColor(111, 9, 176).lighter(170)));
    mac->setBrush(QColor(111, 9, 176));

    // prepare x axis with country labels:
    QVector<double> ticks;//vettore di valori per NUMERO barre
    QVector<QString> labels;//vettore di mac
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);


   connessione();
    ui->customPlot->clearGraphs();
    QSqlQuery *qry1=new QSqlQuery(nDatabase);
    qry1->prepare("select mac,  count(*) AS occorenze from campi group by mac");
    qry1->exec();

    while (qry1->next()) {
            i++;
           QString name = qry1->value(0).toString();
           labels <<  qry1->value(0).toString();
           //qDebug() << name;
           ticks << i;
           macData  << qry1->value(1).toDouble();//altezza barre
           if(qry1->value(1)>max)
            max=qry1->value(1).toInt();

       }


    //QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);
    ui->customPlot->xAxis->setTicker(textTicker);
    ui->customPlot->xAxis->setTickLabelRotation(60);
    ui->customPlot->xAxis->setSubTicks(false);
    ui->customPlot->xAxis->setTickLength(0, 4);
    ui->customPlot->xAxis->setRange(0, (i+1));// Range asse x in base a count distinct mac
    ui->customPlot->xAxis->setBasePen(QPen(Qt::white));
    ui->customPlot->xAxis->setTickPen(QPen(Qt::white));
    ui->customPlot->xAxis->grid()->setVisible(true);
    ui->customPlot->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
    ui->customPlot->xAxis->setTickLabelColor(Qt::white);
    ui->customPlot->xAxis->setLabelColor(Qt::white);

    // prepare y axis:

    ui->customPlot->yAxis->setRange(0, max+2);
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


    mac->setData(ticks, macData);

    // setup legend:
    ui->customPlot->legend->setVisible(true);
    ui->customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignHCenter);
    ui->customPlot->legend->setBrush(QColor(255, 255, 255, 100));
    ui->customPlot->legend->setBorderPen(Qt::NoPen);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    ui->customPlot->legend->setFont(legendFont);

  //  QMouseEvent *e;

   // ui->customPlot_2->setInteraction(QCP::iSelectItems, true);// per selezione singola barra

    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom );//INTERAZIONI POSSIBILI CON IL GRAFICO
    //ui->customPlot->interactions(QCP::Interaction::iSelectItems)


   /*ui->customPlot_2->(QMouseEvent *event)
    {
        if (event->button() == Qt::LeftButton) {
            // handle left mouse button here
        } else {
            // pass on other buttons to base class
            QCheckBox::mousePressEvent(event);
        }
    }
*/
ui->customPlot->replot();

ticks.clear();  //RIMOZIONE GRAFICO NEL CASO DI CLICK AGGIORNAMENTO
labels.clear();
macData.clear();
mac->setData(ticks, macData);
textTicker->addTicks(ticks, labels);

}

void MainWindow::on_Reset_clicked()
{
        //RESET GRAFICO

    connessione();
    ui->customPlot_2->setVisible(false);
    ui->tableView_2->setVisible(false);
    ui->label_7->setVisible(false);
   resetRangeSelect();
}


/*void MainWindow(QCPAbstractPlottable * plottable, int dataIndex, QMouseEvent * event)
{
    qDebug() << "Clicked: " << dataIndex;
}*/
void MainWindow::on_Search_clicked()
{
    QDateTime From=ui->From->dateTime();
    QDateTime To= ui->To->dateTime();

    //agg da qui
    ui->tableView_2->setColumnCount(3);
    auto header = QStringList() << "MAC" << "Prima occorrenza trovata"<< "Ultima occorrenza trovata";
    ui->tableView_2->setHorizontalHeaderLabels(header);
    ui->tableView_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //fino a qui

    ui->tableView_2->setRowCount(0);
    ui->tableView_2->clear();
    ui->tableView_2->reset();
    ui->label_7->setVisible(true);

    QDateTime temp;
    if (From>To){
        QMessageBox msgBox;
        msgBox.setText("La data di inizio di selezione del range risulta maggiore di quella di fine. Si è effettuata la sostituzione automatica delle data di inizio con quella di fine");
        msgBox.exec();
        temp=From;
        From=To;
        To=temp;

    }
    int i=0;// Contatore del numero di Mac distinti
    int max=-1;// valore necessario per settare il range asse y(probabilmente esiste anche funzione automatica)
    double Sottr=0;// valore da sottrarre in caso del count fuori range
    QDateTime maxData(QDate(0000, 1, 1), QTime(0,0,0));
    QDateTime minData= QDateTime::currentDateTime();

    connessione();


    QLinearGradient gradient(0, 0, 0, 400);
    gradient.setColorAt(0, QColor(90, 90, 90));
    gradient.setColorAt(0.38, QColor(105, 105, 105));
    gradient.setColorAt(1, QColor(70, 70, 70));
    ui->customPlot_2->clearGraphs();
    ui->customPlot_2->setBackground(QBrush(gradient));
    //SECONDO GRAFICO
      ui->customPlot_2->setBackground(QBrush(gradient));
      QVector<double> macData2;// altezza barre
      QCPBars *mac2 = new QCPBars(ui->customPlot_2->xAxis, ui->customPlot_2->yAxis);

      //mac2->setSelectable(QCP::stSingleData);// per seezione singola barra
      //connect(ui->customPlot_2, SIGNAL(plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(onPlottableClick(QCPAbstractPlottable*,int,QMouseEvent*)));// per selezione singola barra

      mac2->setAntialiased(false);
      mac2->setStackingGap(1);
      // set names and colors:
      mac2->setName("Indirizzi MAC rilevati");
      mac2->setPen(QPen(QColor(111, 9, 176).lighter(170)));
      mac2->setBrush(QColor(111, 9, 176));

      // prepare x axis with country labels:
      QVector<double> ticks2;//vettore di valori per NUMERO barre
      QVector<QString> labels2;//vettore nome mac

      //QVector<QString> labelsRange;// vettore range rilevamento per QUEL SPECIFICO MAC

      QSqlQuery *qry2=new QSqlQuery(nDatabase);
      QSqlQuery *qryData=new QSqlQuery(nDatabase);

            qry2->prepare("select mac,  count(*), timestamp from campi where timestamp group by mac");

      //qry2->prepare("select mac,  count(*), timestamp AS occorenze from campi where timestamp >= '" + From.toString() + "' AND timestamp<= '"+ To.toString()+"' group by mac");
      qry2->exec();

        QString name2="";

        //INSERIMENTO DATI RANGE RILEVATI NELLA TABELLA(Qtableview2)
        ui->tableView_2->setVisible(true);



         ui->tableView_2->setColumnCount(3);

         ui->tableView_2->setHorizontalHeaderLabels(header);
         bool flag=false;
         while(qry2->next()){//x andare al successivo una volta finito il for

                 flag=false;
                 QDateTime maxData(QDate(1500, 1, 1), QTime(0,0,0));
                 QDateTime minData= QDateTime::currentDateTime();

                 Sottr=qry2->value(1).toDouble();
                 qryData->prepare("select timestamp from campi where mac='"+ qry2->value(0).toString()+ "'");

                 qryData->exec();
                 while(qryData->next())//CASO ES. COUNT MAC ES= 2 MA 1 FUORI RANGE NON VISUALIZZARLO. NECCESSARIO PERCHE' NON RIUSCITO A FARLO NELLA QUERY PER PROBLEMI DI CONVERSIONE toDatatime
                 {

                    if(qryData->value(0).toDateTime()< From || qryData->value(0).toDateTime()>To)
                          Sottr--;

                    if(qryData->value(0).toDateTime()>=From && qryData->value(0).toDateTime()<=To)
                    {

                        flag=true;// nel caso ci sia almeno un valore nel range selezionato
                        if(qryData->value(0).toDateTime()>maxData)// x salvataggio max range e min range di uno specifico mac

                               maxData = qryData->value(0).toDateTime();


                        if(qryData->value(0).toDateTime()< minData)
                         minData = qryData->value(0).toDateTime();
                    }
                 }
                 if(flag==true){
                    i++;
                     name2 = qry2->value(0).toString();
                     labels2 <<  qry2->value(0).toString();//vettore nome mac
                     ticks2 << i;
                     macData2  << Sottr;
                     if(qry2->value(1)>max)
                         max=qry2->value(1).toInt();


                //salvo
                     ui->tableView_2->insertRow(ui->tableView_2->rowCount());
                     ui->tableView_2->setItem(ui->tableView_2->rowCount()-1, 0, new QTableWidgetItem(name2));
                     ui->tableView_2->setItem(ui->tableView_2->rowCount()-1, 1, new QTableWidgetItem(minData.toString()));
                     ui->tableView_2->setItem(ui->tableView_2->rowCount()-1, 2, new QTableWidgetItem(maxData.toString()));
                     ui->tableView_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

                }

        }


    mac2->setData(ticks2, macData2);

      QSharedPointer<QCPAxisTickerText> textTicker2(new QCPAxisTickerText);
      textTicker2->addTicks(ticks2, labels2);

      //QSharedPointer<QCPAxisTickerText> textTickerRange(new QCPAxisTickerText);cercare di mettere range sotto barra dopo il nome
      //textTickerRange->addTicks(ticks2, labelsRange);
      //   ui->customPlot_2->xAxis->setTicker(textTickerRange);

      ui->customPlot_2->xAxis->setTicker(textTicker2);
      ui->customPlot_2->xAxis->setTickLabelRotation(60);
      ui->customPlot_2->xAxis->setSubTicks(true);//
      ui->customPlot_2->xAxis->setTickLength(0, 4);
      ui->customPlot_2->xAxis->setRange(0, (i+1));// Range asse x in base a count distinct mac
      ui->customPlot_2->xAxis->setBasePen(QPen(Qt::white));
      ui->customPlot_2->xAxis->setTickPen(QPen(Qt::white));
      ui->customPlot_2->xAxis->grid()->setVisible(true);
      ui->customPlot_2->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
      ui->customPlot_2->xAxis->setTickLabelColor(Qt::white);
      ui->customPlot_2->xAxis->setLabelColor(Qt::white);

      // prepare y axis:

      ui->customPlot_2->yAxis->setRange(0, max+2);
      ui->customPlot_2->yAxis->setPadding(5); // a bit more space to the left border
      ui->customPlot_2->yAxis->setLabel("Descrizione grafico");
      ui->customPlot_2->yAxis->setBasePen(QPen(Qt::white));
      ui->customPlot_2->yAxis->setTickPen(QPen(Qt::white));
      ui->customPlot_2->yAxis->setSubTickPen(QPen(Qt::white));
      ui->customPlot_2->yAxis->grid()->setSubGridVisible(true);
      ui->customPlot_2->yAxis->setTickLabelColor(Qt::white);
      ui->customPlot_2->yAxis->setLabelColor(Qt::white);
      ui->customPlot_2->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
      ui->customPlot_2->yAxis->grid()->setSubGridPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));

    ui->customPlot_2->replot();
    ui->customPlot_2->setVisible(true);

    //ALLA FINE RESETTO

    ticks2.clear();  //RIMOZIONE GRAFICO NEL CASO DI CLICK SEARCH PER SELEZIONE SUCCESSIVE
    labels2.clear();
    macData2.clear();
    mac2->setData(ticks2, macData2);
    textTicker2->addTicks(ticks2, labels2);
    //


}



void MainWindow::on_Search_2_clicked()
{


    //  APRIRE NUOVA FINESTRA

   Plot3 terzoGraf;
   terzoGraf.setModal(true);
   terzoGraf.exec();
    //ALLA FINE RESETTO


}

void MainWindow::on_Aggior_clicked()
{


    ui->customPlot->legend->clearItems();
    MainWindow::makePlot();
    ui->customPlot_2->setVisible(false);
    ui->tableView_2->setVisible(false);
    ui->label_7->setVisible(false);


    ui->tableView_2->setColumnCount(3);
    auto header = QStringList() << "MAC" << "Prima occorrenza trovata"<< "Ultima occorrenza trovata";
    ui->tableView_2->setHorizontalHeaderLabels(header);
    ui->tableView_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connessione();

    resetRangeSelect();
}
