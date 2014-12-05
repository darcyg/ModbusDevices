#include "DeviceModbus.h"
#include "Convert.h"
#include "ModbusControl.h"


DeviceModbus::DeviceModbus(int sid) : slave_id(sid), abort(false), _modbus_mapping(nullptr)
{
  _modbus_mapping = modbus_mapping_new(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF);
}


DeviceModbus::~DeviceModbus()
{
  switchOff();
  modbus_mapping_free(_modbus_mapping);
}

bool DeviceModbus::load(const Json::Value& json)
{
  return true;
}

bool DeviceModbus::loadRegister(QWidget* w, const Json::Value& json)
{
  if (json["reg"].isArray())
  {
    auto v = json["reg"];
    for (int i = 0; i < json["reg"].size(); i++)
    {
      RegisterModbus reg(w);
      if (!reg.setConfig(json["reg"][i]))
      {
        LOG_ERROR("Register load failed!");
        continue;
      }
      w->setToolTip(reg.toString().c_str());
      _registers.push_back(reg);
    }
  }
  else
  {
    RegisterModbus reg(w);
    if (!reg.setConfig(json))
      return false;
    w->setToolTip(reg.toString().c_str());
    _registers.push_back(reg);
  }
  return true;
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
      reg.value.v_u32 = _modbus_mapping->tab_bits[reg.address()];
      valueToWidget(reg.value, reg.widget());
      break;

    case MODBUS_WRITE_SINGLE_REGISTER:
      _modbus_mapping->tab_input_bits[reg.address()] = (reg.value.v_u32 != 0);
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
      _modbus_mapping->tab_input_bits[reg.address()] = (reg.value.v_u32 != 0);
      break;

    case MODBUS_READ_HOLDING_REGISTERS:
    case MODBUS_READ_INPUT_REGISTERS:
      if (reg.function() == MODBUS_READ_HOLDING_REGISTERS)
        pregs = _modbus_mapping->tab_registers;
      else if (reg.function() == MODBUS_READ_INPUT_REGISTERS)
        pregs = _modbus_mapping->tab_input_registers;
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

void DeviceModbus::switchOn(const char* port, int baudrate)
{
  memset(_modbus_mapping->tab_input_registers, 0, 0xFFFF);
  memset(_modbus_mapping->tab_registers, 0, 0xFFFF);

  if (!ModbusPoll::Instance()->addDevice(std::string(port), baudrate, this))
  {
    MD_THROW("Adding modbus device failed!");
  }
}

void DeviceModbus::switchOff()
{
  ModbusPoll::Instance()->removeDevice(this);
}

/*
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
*/

