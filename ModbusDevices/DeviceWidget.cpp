#include "DeviceWidget.h"
#include <QtUiTools>
#include <QLineEdit>
#include <QCheckBox>
#include <string>
#include <QtSerialPort/QSerialPortInfo>
#include "Settings.hpp"
#include "jsoncpp/json/reader.h"

DeviceWidget::DeviceWidget(QWidget *parent)
: QWidget(parent), in_work(false), modbus_mapping(nullptr), modbus(nullptr)
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

  modbus_mapping_free(this->modbus_mapping);
}

void DeviceWidget::load(const QString& ui_file)
{
  QUiLoader loader;
  
  QFile file(ui_file);
  if (!file.open(QFile::ReadOnly))
    throw std::exception("Can not open file");
  this->widget.reset(loader.load(&file));
  file.close();
  this->setWindowTitle(this->widget->windowTitle());
  ui.scrollArea->setWidget(this->widget.get());
  
  {
    Json::Value value;
    Json::Reader reader;
    this->slave_addr = 0;
    try
    {
      if (reader.parse(std::string("{") + this->widget->styleSheet().toUtf8().constData() + "}", value))
        this->slave_addr = value["addr"].asUInt();
    }
    catch (std::runtime_error& e)
    {
      LOG_ERROR(e.what());
    }
    ui.labelAddress->setText(QString::number(this->slave_addr));
  }

  foreach(QWidget *w, this->widget->findChildren<QWidget*>())
  {
    QString str = w->styleSheet();
    if (str.length())
    {
      LOG_INFO(QString("Parse reg widget: ") + w->objectName());
      Register reg;
      if (!reg.loadJSON(std::string("{") + str.toUtf8().constData() + "}"))
        continue;
      reg.setWidget(w);
      LOG_SUCCESS("func: %d, addr: %d, fmt: %s", reg.function(), reg.address(), reg.format().c_str());
      this->registers.push_back(reg);
    }
  }

  this->modbus_mapping = modbus_mapping_new(0, 0, 0xFFFF, 0xFFFF);
  memset(this->modbus_mapping->tab_input_registers, 0, 0xFFFF);
  memset(this->modbus_mapping->tab_registers, 0, 0xFFFF);

  this->device = std::shared_ptr<Device>(new Device(this, this));

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
{
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
      if (auto obj = qobject_cast<QLineEdit*>(w))
      {
        pregs[preg->address()] = obj->text().toInt();
      }
      else if (auto obj = qobject_cast<QCheckBox*>(w))
      {
        pregs[preg->address()] = obj->isChecked() ? 1 : 0;
      }
    }
    
  }
}

Register* DeviceWidget::findRegister(ushort addr)
{
  for (auto &reg : this->registers)
  {
    if (reg.address() == addr)
      return &reg;
  }
  return nullptr;
}

Register* DeviceWidget::findRegister(const QWidget* widget)
{
  for (auto &reg : this->registers)
  {
    if (reg.widget() == widget)
      return &reg;
  }
  return nullptr;
}

void DeviceWidget::startPoll()
{
  this->modbus = modbus_new_rtu(ui.comboPort->currentText().toLatin1().constData(), 115200, 'N', 8, 1);
  if (!this->modbus)
    throw std::runtime_error("modbus_new_rtu failed");

  modbus_set_slave(this->modbus, this->slave_addr);

  timeval tv = { 0, 1000 };
  modbus_set_response_timeout(this->modbus, &tv);
  if (modbus_connect(this->modbus) == -1)
  {
    modbus_free(this->modbus);
    ui.pushStart->setText("Start");
    this->in_work = false;
    throw std::runtime_error(std::string("Connection failed: ") + modbus_strerror(errno));
  }
  else
  {
    ui.pushStart->setText("Stop");
    this->in_work = true;
  }
  ui.comboPort->setEnabled(!this->in_work);
  this->device->switchOn();
}

void DeviceWidget::stopPoll()
{
  this->device->switchOff();
  modbus_close(this->modbus);
  modbus_free(this->modbus);
  this->modbus = nullptr;
  this->in_work = false;
  ui.pushStart->setText("Start");
  ui.comboPort->setEnabled(true);
}

void DeviceWidget::poll()
{
  if (!this->in_work)
    return;
  if (modbus_receive(this->modbus, this->query) == -1)
  {
    //LOG_ERROR("modbus_receive FAILED");
    return;
  }
  switch (this->query[modbus_get_header_length(this->modbus)])
  {
  case MODBUS_READ_INPUT_REGISTERS: {
    LOG_INFO("MODBUS_READ_INPUT_REGISTERS");
    modbus_reply(this->modbus, this->query, 0, this->modbus_mapping);
  } break;
  case MODBUS_READ_HOLDING_REGISTERS:
    LOG_INFO("MODBUS_READ_HOLDING_REGISTERS");
    break;
  default:
    LOG_ERROR("UNKNOWN FUNCTION");
    break;
  }
}
