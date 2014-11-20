#ifndef DEVICEWIDGET_H
#define DEVICEWIDGET_H

#include <QWidget>
#include "ui_DeviceWidget.h"

class DeviceWidget : public QWidget
{
    Q_OBJECT

public:
    DeviceWidget(QWidget *parent = 0);
    ~DeviceWidget();

private:
    Ui::DeviceWidget ui;
};

#endif // DEVICEWIDGET_H
