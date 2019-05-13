#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include <QMouseEvent> //per click su grafico a barre

#include <QtSql/QSqlDatabase>
namespace Ui {
class MainWindow;
}

class   QSqlTableModel;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
   // void mousePressEvent(QMouseEvent *ev);
    ~MainWindow();

private slots:
    void makePlot();
    void on_Reset_clicked();
  //  void Mouse_Pressed();
    void on_Search_clicked();

    void   connessione();
    void resetRangeSelect();

    void on_Search_2_clicked();

    void on_Aggior_clicked();

private:
    Ui::MainWindow *ui;
    QSqlTableModel *nModel;
    QSqlDatabase nDatabase;


};

#endif // MAINWINDOW_H
