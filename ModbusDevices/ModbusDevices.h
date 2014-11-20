#ifndef MODBUSDEVICES_H
#define MODBUSDEVICES_H

#include <QtWidgets/QMainWindow>
#include "ui_ModbusDevices.h"

class ModbusDevices : public QMainWindow
{
    Q_OBJECT

public:
    ModbusDevices(QWidget *parent = 0);
    ~ModbusDevices();

private:
    Ui::ModbusDevicesClass ui;
};

#endif // MODBUSDEVICES_H
