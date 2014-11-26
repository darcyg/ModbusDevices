#include "DeviceModbus.h"
#include "Convert.h"


DeviceModbus::DeviceModbus(int sid) : slave_id(sid), abort(false), modbus_mapping(nullptr), modbus(nullptr)
{
  this->modbus_mapping = modbus_mapping_new(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF);
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

bool DeviceModbus::loadRegister(QWidget* w, const Json::Value& json)
{
  RegisterModbus reg(w);
  if (!reg.setConfig(json))
    return false;
  w->setToolTip(reg.toString().c_str());
  _registers.push_back(reg);
  return true;
}

void DeviceModbus::run()
{
  while (!this->abort)
  {
    int recv_bytes = modbus_receive(this->modbus, this->query);
    if (recv_bytes == -1)
    {
      //LOG_ERROR("modbus_receive FAILED");
      continue;
    }
    updateValues();
    switch (this->query[modbus_get_header_length(this->modbus)])
    {
    case MODBUS_READ_COILS:
      LOG_INFO("MODBUS_READ_COILS");
      modbus_reply(this->modbus, this->query, recv_bytes, this->modbus_mapping);
      break;

    case MODBUS_READ_DISCRETE_INPUTS:
      LOG_INFO("MODBUS_READ_DISCRETE_INPUTS");
      modbus_reply(this->modbus, this->query, 0, this->modbus_mapping);
      break;

    case MODBUS_READ_INPUT_REGISTERS:
      LOG_INFO("MODBUS_READ_INPUT_REGISTERS");
      modbus_reply(this->modbus, this->query, recv_bytes, this->modbus_mapping);
      break;

    case MODBUS_READ_HOLDING_REGISTERS:
      LOG_INFO("MODBUS_READ_HOLDING_REGISTERS");
      modbus_reply(this->modbus, this->query, recv_bytes, this->modbus_mapping);
      break;

    case MODBUS_WRITE_SINGLE_COIL:
      LOG_INFO("MODBUS_WRITE_SINGLE_COIL");
      modbus_reply(this->modbus, this->query, recv_bytes, this->modbus_mapping);
      updateWidgets();
      break;

    case MODBUS_WRITE_SINGLE_REGISTER:
      LOG_INFO("MODBUS_WRITE_SINGLE_REGISTER");
      modbus_reply(this->modbus, this->query, recv_bytes, this->modbus_mapping);
      break;

    case MODBUS_WRITE_MULTIPLE_COILS:
      LOG_INFO("MODBUS_WRITE_MULTIPLE_COILS");
      break;

    case MODBUS_WRITE_MULTIPLE_REGISTERS:
      LOG_INFO("MODBUS_WRITE_MULTIPLE_REGISTERS");
      break;

    default:
      LOG_ERROR("UNKNOWN FUNCTION");
      break;
    }
  }
}

void DeviceModbus::updateWidgets()
{
  for (auto& reg : _registers)
  {
    uint16_t* pregs = nullptr;
    uint8_t* pregs8 = nullptr;

    switch (reg.function())
    {
    case MODBUS_WRITE_SINGLE_COIL:
      reg.value.v_u32 = this->modbus_mapping->tab_bits[reg.address()];
      valueToWidget(reg.value, reg.widget());
      break;

    case MODBUS_WRITE_SINGLE_REGISTER:
      this->modbus_mapping->tab_input_bits[reg.address()] = (reg.value.v_u32 != 0);
      break;

    case MODBUS_WRITE_MULTIPLE_COILS:
    case MODBUS_WRITE_MULTIPLE_REGISTERS:
      
      break;
    default:
      continue;
    }
  }
}

void DeviceModbus::updateValues()
{
  for (auto& reg : _registers)
  {
    uint16_t* pregs = nullptr;
    uint8_t* pregs8 = nullptr;

    if (reg.function() > MODBUS_READ_INPUT_REGISTERS)
      continue;

    widgetToValue(reg.widget(), reg.value);

    switch (reg.function())
    {
    case MODBUS_READ_COILS:
      break;

    case MODBUS_READ_DISCRETE_INPUTS:
      this->modbus_mapping->tab_input_bits[reg.address()] = (reg.value.v_u32 != 0);
      break;

    case MODBUS_READ_HOLDING_REGISTERS:
    case MODBUS_READ_INPUT_REGISTERS:
      if (reg.function() == MODBUS_READ_HOLDING_REGISTERS)
        pregs = this->modbus_mapping->tab_registers;
      else if (reg.function() == MODBUS_READ_INPUT_REGISTERS)
        pregs = this->modbus_mapping->tab_input_registers;
      if (pregs)
        pregs[reg.address()] = reg.value.v_u32;
      if (pregs8)
        pregs8[reg.address()] = reg.value.v_u32 ? 1 : 0;
      if (reg.address_hi() != -1 && pregs)
        pregs[reg.address_hi()] = reg.value.v_u32 >> 16;
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


RegisterModbus* DeviceModbus::findRegister(ushort addr)
{
  for (auto &reg : this->_registers)
  {
    if (reg.address() == addr)
      return &reg;
  }
  return nullptr;
}

RegisterModbus* DeviceModbus::findRegister(const QWidget* widget)
{
  for (auto &reg : this->_registers)
  {
    if (reg.widget() == widget)
      return &reg;
  }
  return nullptr;
}


