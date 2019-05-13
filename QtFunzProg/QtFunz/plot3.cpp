#include "plot3.h"
#include "ui_plot3.h"
#include "mainwindow.h"
#include <QSqlQuery>
#include<QSqlTableModel>
#include<QSqlError>
#include <QtSql>

#include <QtDataVisualization/Q3DScene>

void Plot3::connessione(){
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


}

Plot3::Plot3(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Plot3)
{
    ui->setupUi(this);
   ui->customPlot_3->setVisible(false);
   ui->label_2->setVisible(false);
   ui->comboBox->setVisible(false);
   ui->pushButton->setVisible(false);
   connessione();

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

        qDebug()<<data;
      }



 // QDateTimeEdit *dateEdit = new QDateTimeEdit(QDate::currentDate());
//  dateEdit->setDisplayFormat("yyyy.MM.dd");
   ui->From_2->setMaximumDateTime(maxData);
   ui->From_2->setMinimumDateTime(minData);

   ui->To_2->setMinimumDateTime(minData);
   ui->To_2->setMaximumDateTime(QDateTime::currentDateTime());// da vedere se metterlo veramente come vincolo
}

Plot3::~Plot3()
{
    delete ui;
}

void Plot3::on_pushButton_clicked()// Tasto finle per visualizzazione grafico. Necessario perchè scelta mac da comboBox successiva a scelta range data
{
    QString macCombo;// mac selezionato dalla ComboBox
    macCombo=ui->comboBox->currentText();//Salvo mac selezionato
    ui->customPlot_3->setVisible(true);


    //PROVA 3D



}

void Plot3::on_Search_2_clicked()// Da fare tipo di grafico diverso
{
    ui->label_2->setVisible(true);
    ui->comboBox->setVisible(true);
    ui->pushButton->setVisible(true);

    ui->comboBox->clear();//reset valori inseriti nella combo box precedentemente

    QDateTime From=ui->From_2->dateTime();
    QDateTime To= ui->To_2->dateTime();

    qDebug()<<From;
   // qDebug()<<From.toString();



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
  ui->customPlot_3->clearGraphs();
  ui->customPlot_3->setBackground(QBrush(gradient));
  //SECONDO GRAFICO
    ui->customPlot_3->setBackground(QBrush(gradient));
    QVector<double> macData2;// altezza barre
    QCPBars *mac2 = new QCPBars(ui->customPlot_3->xAxis, ui->customPlot_3->yAxis);


    mac2->setAntialiased(false);
    mac2->setStackingGap(1);
    // set names and colors:
    mac2->setName("Indirizzi MAC rilevati");
    mac2->setPen(QPen(QColor(111, 9, 176).lighter(170)));
    mac2->setBrush(QColor(111, 9, 176));

    // prepare x axis with country labels:
    QVector<double> ticks2;//vettore di valori per NUMERO barre
    QVector<QString> labels2;//vettore nome mac


    QSqlQuery *qry2=new QSqlQuery(nDatabase);
    QSqlQuery *qryData=new QSqlQuery(nDatabase);

          qry2->prepare("select mac,  count(*), timestamp from campi where timestamp group by mac");

    qry2->exec();

      QString name2="";


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
                    ui->comboBox->addItem(qry2->value(0).toString());


              }

      }
       mac2->setData(ticks2, macData2);

         QSharedPointer<QCPAxisTickerText> textTicker2(new QCPAxisTickerText);
         textTicker2->addTicks(ticks2, labels2);

         ui->customPlot_3->xAxis->setTicker(textTicker2);
         ui->customPlot_3->xAxis->setTickLabelRotation(60);
         ui->customPlot_3->xAxis->setSubTicks(true);//
         ui->customPlot_3->xAxis->setTickLength(0, 4);
         ui->customPlot_3->xAxis->setRange(0, (i+1));// Range asse x in base a count distinct mac
         ui->customPlot_3->xAxis->setBasePen(QPen(Qt::white));
         ui->customPlot_3->xAxis->setTickPen(QPen(Qt::white));
         ui->customPlot_3->xAxis->grid()->setVisible(true);
         ui->customPlot_3->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
         ui->customPlot_3->xAxis->setTickLabelColor(Qt::white);
         ui->customPlot_3->xAxis->setLabelColor(Qt::white);

         // prepare y axis:

         ui->customPlot_3->yAxis->setRange(0, max+2);
         ui->customPlot_3->yAxis->setPadding(5); // a bit more space to the left border
         ui->customPlot_3->yAxis->setLabel("Descrizione grafico");
         ui->customPlot_3->yAxis->setBasePen(QPen(Qt::white));
         ui->customPlot_3->yAxis->setTickPen(QPen(Qt::white));
         ui->customPlot_3->yAxis->setSubTickPen(QPen(Qt::white));
         ui->customPlot_3->yAxis->grid()->setSubGridVisible(true);
         ui->customPlot_3->yAxis->setTickLabelColor(Qt::white);
         ui->customPlot_3->yAxis->setLabelColor(Qt::white);
         ui->customPlot_3->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
         ui->customPlot_3->yAxis->grid()->setSubGridPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));

       ui->customPlot_3->replot();
       ui->customPlot_3->setVisible(true);

       ticks2.clear();  //RIMOZIONE GRAFICO NEL CASO DI CLICK SEARCH PER SELEZIONE SUCCESSIVE
       labels2.clear();
       macData2.clear();
       mac2->setData(ticks2, macData2);
       textTicker2->addTicks(ticks2, labels2);



}

void Plot3::on_Reset_2_clicked()
{
    connessione();
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

         qDebug()<<data;
       }
    ui->From_2->setMaximumDateTime(maxData);
    ui->From_2->setMinimumDateTime(minData);

    ui->To_2->setMinimumDateTime(minData);
    ui->To_2->setMaximumDateTime(QDateTime::currentDateTime());// da vedere se metterlo veramente come vincolo

    ui->From_2->setDateTime(minData);
    ui->To_2->setDateTime(maxData);
}
