#ifndef DEVICEWIDGET_H
#define DEVICEWIDGET_H

#include <QWidget>
#include <QtSerialPort/QtSerialPort>
#include "ui_DeviceWidget.h"
#include "LoggerWidget.h"
#include "Register.h"
#include "Device.h"
#include <memory>
#include "libmodbus/modbus.h"

class Device;

class DeviceWidget : public QWidget
{
  Q_OBJECT

public:
  DeviceWidget(QWidget *parent=0);
  ~DeviceWidget();
  void load(const QString& ui_file);
  void poll();
  void stopPoll();
  void startPoll();
  Register* findRegister(ushort addr);
  Register* findRegister(const QWidget* widget );
  int address() const { return slave_addr; }

public slots:
  void pushOpen_clicked();
  
protected:
  virtual void timerEvent(QTimerEvent *event);

private:
  Ui::DeviceWidget          ui;
  std::shared_ptr<QWidget>  widget;
  std::shared_ptr<Device>   device;
  std::vector<Register>     registers;
  modbus_t*                 modbus;
  modbus_mapping_t*         modbus_mapping;
  
  int                       slave_addr;
  bool                      in_work;
  unsigned char             query[MODBUS_RTU_MAX_ADU_LENGTH];
};

#endif // DEVICEWIDGET_H
