#ifndef DEVICEWIDGET_H
#define DEVICEWIDGET_H

#include <QWidget>
#include "ui_DeviceWidget.h"
#include "LoggerWidget.h"
#include "Register.h"
#include <memory>

class DeviceWidget : public QWidget
{
  Q_OBJECT

public:
  DeviceWidget(QWidget *parent);
  ~DeviceWidget();
  void load(const QString& ui_file);

private:
  Ui::DeviceWidget          ui;
  std::shared_ptr<QWidget>  widget;
  std::vector<Register>     registers;
};

#endif // DEVICEWIDGET_H
