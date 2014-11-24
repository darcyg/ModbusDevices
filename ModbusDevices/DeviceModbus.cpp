#include "DeviceModbus.h"
#include "IncludeMe.h"


DeviceModbus::DeviceModbus(int sid) : slave_id(sid), abort(false), modbus_mapping(nullptr), modbus(nullptr)
{
  this->modbus_mapping = modbus_mapping_new(0, 0, 0xFFFF, 0xFFFF);
}


DeviceModbus::~DeviceModbus()
{
  switchOff();
  modbus_mapping_free(this->modbus_mapping);
}

bool DeviceModbus::load(const Json::Value& json)
{
  return true;
}

void DeviceModbus::run()
{
  while (!this->abort)
  {
    if (modbus_receive(this->modbus, this->query) == -1)
    {
      //LOG_ERROR("modbus_receive FAILED");
      continue;
    }
    switch (this->query[modbus_get_header_length(this->modbus)])
    {
    case MODBUS_READ_INPUT_REGISTERS:
      LOG_INFO("MODBUS_READ_INPUT_REGISTERS");
      modbus_reply(this->modbus, this->query, 0, this->modbus_mapping);
      break;
    case MODBUS_READ_HOLDING_REGISTERS:
      LOG_INFO("MODBUS_READ_HOLDING_REGISTERS");
      modbus_reply(this->modbus, this->query, 0, this->modbus_mapping);
      break;
    default:
      LOG_ERROR("UNKNOWN FUNCTION");
      break;
    }
  }
}

void DeviceModbus::switchOn(const char* port)
{
  memset(this->modbus_mapping->tab_input_registers, 0, 0xFFFF);
  memset(this->modbus_mapping->tab_registers, 0, 0xFFFF);

  this->modbus = modbus_new_rtu(port, 115200, 'N', 8, 1);
  if (!this->modbus)
    MD_THROW("modbus_new_rtu failed");

  modbus_set_slave(this->modbus, this->slave_id);
  timeval tv = { 0, 1000 };
  modbus_set_response_timeout(this->modbus, &tv);

  if (modbus_connect(this->modbus) == -1)
  {
    modbus_free(this->modbus);
    MD_THROW(std::string("Connection failed: ") + modbus_strerror(errno));
  }

  this->abort = false;
  this->start();
}

void DeviceModbus::switchOff()
{
  if (!this->isRunning())
    return;
  this->abort = true;
  this->wait();
  modbus_close(this->modbus);
  modbus_free(this->modbus);
}


Register* DeviceModbus::findRegister(ushort addr)
{
  for (auto &reg : this->_registers)
  {
    if (reg.address() == addr)
      return &reg;
  }
  return nullptr;
}

Register* DeviceModbus::findRegister(const QWidget* widget)
{
  for (auto &reg : this->_registers)
  {
    if (reg.widget() == widget)
      return &reg;
  }
  return nullptr;
}