#include "DeviceWidget.h"
#include <QtUiTools>
#include <QLineEdit>
#include <QCheckBox>
#include <string>
#include <QtSerialPort/QSerialPortInfo>
#include "Settings.hpp"
#include "jsoncpp/json/reader.h"
#include "DeviceModbus.h"
#include "DeviceTura.h"
#include "IncludeMe.h"

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
  this->startTimer(200);
}

DeviceWidget::~DeviceWidget()
{
  SETTINGS->beginGroup(windowTitle());
  SETTINGS->setValue("port", ui.comboPort->currentText());
  SETTINGS->sync();
  SETTINGS->endGroup();
  if (this->device)
    this->device->save(windowTitle());
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
    if (!this->device->load(value))
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

  this->device->load(windowTitle());

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
  catch (std::runtime_error& e)
  {
    LOG_ERROR(e.what());
  }
}

void DeviceWidget::timerEvent(QTimerEvent *event)
{/*
  foreach(QWidget *w, this->widget->findChildren<QWidget*>())
  {
    Register* preg = findRegister(w);
    if (preg)
    {
      uint16_t* pregs = nullptr;
      if (preg->function() == MODBUS_READ_HOLDING_REGISTERS)
        pregs = this->modbus_mapping->tab_registers;
      else if (preg->function() == MODBUS_READ_INPUT_REGISTERS)
        pregs = this->modbus_mapping->tab_input_registers;
      else
        continue;
      preg->widgetToValue();
      pregs[preg->address()] = preg->value.v_u32;
    }
    
  }*/
}


bool DeviceWidget::startPoll()
{
  this->device->switchOn(ui.comboPort->currentText().toLatin1().constData());
  ui.pushStart->setText("Stop");
  this->in_work = true;
  ui.comboPort->setEnabled(!this->in_work);
  return true;
}

bool DeviceWidget::stopPoll()
{
  this->device->switchOff();
  this->in_work = false;
  ui.pushStart->setText("Start");
  ui.comboPort->setEnabled(true);
  return true;
}
