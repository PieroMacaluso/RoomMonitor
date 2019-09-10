//
// Created by pieromack on 10/09/19.
//

#ifndef ROOMMONITOR_SETTINGDIALOG_H
#define ROOMMONITOR_SETTINGDIALOG_H


#include <QtWidgets/QDialog>
#include "../ui_settings.h"
#include <QtCore/QSettings>


class SettingDialog : public QDialog {
Q_OBJECT
public:
    Ui::ConfigDialog ui{};
    QSettings s{};

    SettingDialog();

    void settingCheckUp();

    void setupConnect();

public slots:

    void apply() {
        s.setValue("monitor/A", ui.aEdit->text().toFloat());
        s.setValue("monitor/n", ui.nEdit->text().toFloat());
        s.setValue("room/width", ui.widthEdit->text().toFloat());
        s.setValue("room/height", ui.heightEdit->text().toFloat());
        this->close();
    }

};


#endif //ROOMMONITOR_SETTINGDIALOG_H
