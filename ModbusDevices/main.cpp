#include "ModbusDevices.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ModbusDevices w;
    w.show();
    return a.exec();
}
