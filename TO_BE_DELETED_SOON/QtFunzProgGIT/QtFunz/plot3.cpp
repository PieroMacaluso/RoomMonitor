#include "plot3.h"
#include "ui_plot3.h"
#include "mainwindow.h"
#include <QSqlQuery>
#include<QSqlTableModel>
#include<QSqlError>
#include <QtSql>

#include <QtDataVisualization/Q3DScene>
#include <QtCharts/QChartView>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QLineSeries>
//#include <QValueAxis>

QT_CHARTS_USE_NAMESPACE

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
   // ui->label_IMM->setVisible(true);
  //  QPixmap pix("./tel2.png");
    //ui->label_IMM->setPixmap(pix);


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

void Plot3::on_pushButton_clicked()// Tasto finale per visualizzazione grafico. Necessario perchè scelta mac da comboBox successiva a scelta range data
{
    QString macCombo;// mac selezionato dalla ComboBox
    macCombo=ui->comboBox->currentText();//Salvo mac selezionato
    ui->customPlot_3->setVisible(true);
    connessione();
  //   ui->customPlot_3->clearGraphs();
    ui->customPlot_3->axisRect()->setBackground(QPixmap("./stanza2.jpg"));// immagine background per ora no
    ui->customPlot_3->addGraph();
    ui->customPlot_3->graph()->setLineStyle(QCPGraph::lsStepCenter);
    QPen pen;
    pen.setColor(QColor(255, 200, 20, 200));
    pen.setStyle(Qt::DashLine);
    pen.setWidthF(2.5);
    ui->customPlot_3->graph()->setPen(pen);
    ui->customPlot_3->graph()->setBrush(QBrush(QColor(255,200,20,70)));
    ui->customPlot_3->graph()->setScatterStyle(QCPScatterStyle(QPixmap("./tel2.png")));// immagine telefono punto per ora no
    // set graph name, will show up in legend next to icon:
    //ui->customPlot_3->graph()->setName("Posizione");//LEGENDA
    // set data:
    QVector<double> CordX;
    QVector<double> CordY;
  //  AGGIUNTO
   // QLineSeries *series = new QLineSeries();
  //


    //valore iniziale 0 entrata???? oppure non metterlo
    QCPGraph *graph1 = ui->customPlot_3->addGraph();
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    QVector<QString> labelsX;
    QVector<QString> labelsY;

    double maxY=-1;// valore necessario per settare il range asse y(probabilmente esiste anche funzione automatica) FORSE LO IMPOSTIAMO NOI IN BASE ALLA DIMENSIONE DELLA STANZA
    double maxX=-1;
     QSqlQuery *qryOut=new QSqlQuery(nDatabase);
     qryOut->prepare("select mac,  posX, posY, timestamp from campi where mac='"+ macCombo + "' order by timestamp");
     qryOut->exec();
     while(qryOut->next()){
         if(qryOut->value(1)>maxX)// Setto range massino x e y FORSE NON SERVE PERCHE SETTATI CON DIMENSIONE STANZA
             maxX=qryOut ->value(1).toDouble();
         if(qryOut->value(1)>maxY)
             maxY=qryOut ->value(2).toDouble();
   // qDebug()<<CordX;
    //qDebug()<<CordY;
         CordX << qryOut->value(1).toDouble();
         CordY << qryOut->value(2).toDouble();
//AGGIUNTO
         // series->append(qryOut->value(1).toDouble(), qryOut->value(2).toDouble());
//


         labelsX <<  qryOut->value(1).toString();// imposto il valore esatto del punto
         labelsY <<  qryOut->value(2).toString();// imposto il valore esatto del punto
     }

   // QPainter PER L'ORDINE CORRETTO
      graph1->setData(CordX, CordY);
      graph1->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(Qt::white), 9));
      graph1->setPen(QPen(QColor(120, 120, 120), 2));

      //AGGIUNTO
   /*   QChart *chart = new QChart();
        chart->legend()->hide();
        chart->addSeries(series);
        // QValueAxis *axisX = new QValueAxis;
        //axisX->setRange(4, 6);
        //chart->addAxis(axisX, Qt::AlignRight);
        chart->createDefaultAxes();
        chart->setTitle("Simple line chart example");*/



      textTicker->addTicks( CordX, labelsX);// aggiungi valori precisi
      ui->customPlot_3->xAxis->setTicker(textTicker);

      textTicker->addTicks( CordY, labelsY);
      ui->customPlot_3->yAxis->setTicker(textTicker);

/*utile
     ui->customPlot_3->rescaleAxes();
*/

    ui->customPlot_3->graph()->setData(CordX, CordY);// CONTIENE I VALORI DELLE COORDINATE X E Y DA INSERIRE NEL GRAFICO

    // set title of plot:
    //ui->customPlot_3->plotLayout()->insertRow(0);
    //ui->customPlot_3->plotLayout()->addElement(0, 0, new QCPTextElement(ui->customPlot_3, "Prova", QFont("sans", 12, QFont::Bold)));

    // axis configurations:
    ui->customPlot_3->xAxis->setLabel("Cordinata X posizione dispositivo (metri)");
    ui->customPlot_3->yAxis->setLabel("Cordinata Y posizione dispositivo (metri)");
    ui->customPlot_3->xAxis2->setVisible(true);
    ui->customPlot_3->yAxis2->setVisible(true);
    ui->customPlot_3->xAxis2->setTickLabels(false);
    ui->customPlot_3->yAxis2->setTickLabels(false);
    ui->customPlot_3->xAxis2->setTicks(false);
    ui->customPlot_3->yAxis2->setTicks(false);
    ui->customPlot_3->xAxis2->setSubTicks(false);
    ui->customPlot_3->yAxis2->setSubTicks(false);

     ui->customPlot_3->xAxis->setRange(0, maxX+2);// Range asse x in base a count distinct mac
     ui->customPlot_3->yAxis->setRange(0, maxY+2);

    // setup legend:
    //ui->customPlot_3->legend->setFont(QFont(font().family(), 7));
    //ui->customPlot_3->legend->setIconSize(50, 20);
    //ui->customPlot_3->legend->setVisible(true);
    //ui->customPlot_3->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft | Qt::AlignTop);

    ui->customPlot_3->replot();
    ui->customPlot_3->update();
    CordX.clear();  //RIMOZIONE GRAFICO NEL CASO DI CLICK AGGIORNAMENTO
    CordY.clear();
    labelsX.clear();
    labelsY.clear();
    graph1->setData(CordX, CordY);
    textTicker->addTicks(CordX, labelsX);
    textTicker->addTicks(CordY, labelsY);

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

 connessione();

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


               qryData->prepare("select timestamp from campi where mac='"+ qry2->value(0).toString()+ "'");

               qryData->exec();
               while(qryData->next())//CASO ES. COUNT MAC ES= 2 MA 1 FUORI RANGE NON VISUALIZZARLO. NECCESSARIO PERCHE' NON RIUSCITO A FARLO NELLA QUERY PER PROBLEMI DI CONVERSIONE toDatatime
               {

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
                 // i++;
                   name2 = qry2->value(0).toString();

                    ui->comboBox->addItem(qry2->value(0).toString());


              }
               //else{
                //Nessun dispositivo presente nel range selezionato. Cambia range. ->setVisible=false
                //}

      }




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
