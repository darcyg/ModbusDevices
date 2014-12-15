#include "DeviceWidget.h"
#include <QtUiTools>
#include <QLineEdit>
#include <QCheckBox>
#include <string>
#include <QtSerialPort/QSerialPortInfo>
#include "Settings.hpp"
#include "jsoncpp/json/reader.h"
#include "DeviceModbus/DeviceModbus.h"
#include "DeviceTura/DeviceTura.h"
#include "IncludeMe.h"
#include "include/Python.h"

static Device* createDevice(const std::string& cls, int slave_id)
{
  if (cls == "DeviceModbus")
    return new DeviceModbus(slave_id);
  else if (cls == "DeviceTura")
    return new DeviceTura();
  return nullptr;
}

DeviceWidget::DeviceWidget(QWidget *parent)
: QWidget(parent), in_work(false)
{
  ui.setupUi(this);

  connect(ui.pushStart, SIGNAL(clicked()), this, SLOT(pushOpen_clicked()));

  foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
  {
    ui.comboPort->insertItem(0, info.portName());
  }
  
}

DeviceWidget::~DeviceWidget()
{
  SETTINGS->beginGroup(windowTitle());
  SETTINGS->setValue("port", ui.comboPort->currentText());
  SETTINGS->sync();
  SETTINGS->endGroup();
  if (this->device)
    this->device->saveState(windowTitle());
}

void DeviceWidget::load(const QString& ui_file)
{
  QUiLoader loader;
  
  QFile file(ui_file);
  if (!file.open(QFile::ReadOnly))
    MD_THROW("Can not open file");
  this->widget = loader.load(&file);
  file.close();
  this->setWindowTitle(this->widget->windowTitle());
  ui.scrollArea->setWidget(this->widget);
  
  {
    Json::Value value;
    Json::Reader reader;
    
    if (!reader.parse(std::string("{") + this->widget->styleSheet().toUtf8().constData() + "}", value))
      MD_THROW("Could not parse widget config file");
    
    this->device.reset(createDevice(value["class"].asString(), value["addr"].asUInt()));
    if (!this->device)
      MD_THROW("Unknown device class");
    if (!this->device->load(value, ui_file))
      MD_THROW("Load device failed");
    
    ui.labelAddress->setText(this->device->caption());
  }

  foreach(QWidget *w, this->widget->findChildren<QWidget*>())
  {
    QString str = w->styleSheet();
    if (str.length())
    {
      Json::Value value;
      Json::Reader reader;
      if (!reader.parse(std::string("{") + str.toUtf8().constData() + "}", value))
      {
        LOG_ERROR("Parse failed\n%s", value.toStyledString().c_str());
        continue;
      }
      this->device->loadRegister(w, value);
    }
  }

  this->device->loadState(windowTitle());

  auto a = windowTitle();
  SETTINGS->beginGroup(windowTitle());
  QString port = SETTINGS->value("port").toString();
  SETTINGS->endGroup();
  int idx = ui.comboPort->findText(port);
  if (idx != -1)
    ui.comboPort->setCurrentIndex(idx);
}

void DeviceWidget::pushOpen_clicked()
{
  try
  {
    if (this->in_work)
      stopPoll();
    else
      startPoll();
  }
  catch (md_exception& e)
  {
    LOG_ERROR(e.what());
  }
}

bool DeviceWidget::startPoll()
{
  QString port("\\\\.\\");
  port.append(ui.comboPort->currentText());
  this->device->switchOn(port.toLatin1().constData(), ui.comboBaudrate->currentText().toInt());
  ui.pushStart->setText("Stop");
  this->in_work = true;
  ui.comboPort->setEnabled(!this->in_work);
  ui.comboBaudrate->setEnabled(!this->in_work);
  return true;
}

bool DeviceWidget::stopPoll()
{
  this->device->switchOff();
  this->in_work = false;
  ui.pushStart->setText("Start");
  ui.comboPort->setEnabled(true);
  ui.comboBaudrate->setEnabled(true);
  return true;
}

void DeviceWidget::timerEvent(QTimerEvent * e)
{

}
