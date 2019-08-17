#ifndef PLOT3_H
#define PLOT3_H

#include <QDialog>
#include <QtSql/QSqlDatabase>
namespace Ui {
class Plot3;
}
class   QSqlTableModel;
class Plot3 : public QDialog
{
    Q_OBJECT

public:
    explicit Plot3(QWidget *parent = nullptr);
    ~Plot3();

private slots:
    void on_pushButton_clicked();

    void on_Search_2_clicked();

    void on_Reset_2_clicked();
     void   connessione();

private:
    Ui::Plot3 *ui;
    QSqlTableModel *nModel;
    QSqlDatabase nDatabase;
};

#endif // PLOT3_H
