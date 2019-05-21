#ifndef PRINC_H
#define PRINC_H

#include <QDialog>

namespace Ui {
class Princ;
}

class Princ : public QDialog
{
    Q_OBJECT

public:
    explicit Princ(QWidget *parent = nullptr);
    ~Princ();

private slots:
    void on_Next_clicked();

private:
    Ui::Princ *ui;
};

#endif // PRINC_H
