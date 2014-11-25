#ifndef DEVICEWIDGET_H
#define DEVICEWIDGET_H

#include <QWidget>
#include <QtSerialPort/QtSerialPort>
#include "ui_DeviceWidget.h"
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
  void stopPoll();
  void startPoll(); 
  
public slots:
  void pushOpen_clicked();
  
protected:
  virtual void timerEvent(QTimerEvent *event);

private:
  Ui::DeviceWidget          ui;
  QWidget*                  widget;
  std::shared_ptr<Device>   device;
    
  bool                      in_work;
  
};

#endif // DEVICEWIDGET_H
