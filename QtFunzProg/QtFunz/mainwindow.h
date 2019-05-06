#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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
    ~MainWindow();

private slots:
    void makePlot();
private:
    Ui::MainWindow *ui;
    QSqlTableModel *nModel;
    QSqlDatabase nDatabase;
};

#endif // MAINWINDOW_H
