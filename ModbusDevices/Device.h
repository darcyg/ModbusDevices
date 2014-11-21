#ifndef DEVICE_H
#define DEVICE_H

#include <QThread>
#include "DeviceWidget.h"
#include "libmodbus/modbus.h"

class DeviceWidget;

class Device : public QThread
{
  Q_OBJECT

public:
  Device(QObject *parent, DeviceWidget* widget);
  ~Device();

  void switchOn();
  void switchOff();

protected:
  virtual void run();

private:
  DeviceWidget*     widget;
  bool              abort;
  modbus_t*         modbus;
};

#endif // DEVICE_H
