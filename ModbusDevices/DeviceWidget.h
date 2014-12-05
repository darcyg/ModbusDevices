#ifndef DEVICEWIDGET_H
#define DEVICEWIDGET_H

#include <QWidget>
#include <QtSerialPort/QtSerialPort>
#include "ui_DeviceWidget.h"
#include "Device.h"
#include <memory>

class Device;

class DeviceWidget : public QWidget
{
  Q_OBJECT

public:
  DeviceWidget(QWidget *parent=0);
  ~DeviceWidget();
  void load(const QString& ui_file);
  bool stopPoll();
  bool startPoll();
  bool inWork() const { return in_work;  }
  
signals:
  void started();
  void stopped();

public slots:
  void pushOpen_clicked();
  
protected:
  void timerEvent(QTimerEvent * e);

private:
  Ui::DeviceWidget          ui;
  QWidget*                  widget;
  std::shared_ptr<Device>   device;
    
  bool                      in_work;
  
};

#endif // DEVICEWIDGET_H
