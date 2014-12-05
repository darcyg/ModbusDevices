#include "ModbusControl.h"
#include <QThread>

ModbusPoll* g_poll = ModbusPoll::Instance();

Modbus::Modbus(std::string &port, int baudrate) : _port_name(port), _baudrate(baudrate)
{
  
}

DeviceModbus* Modbus::findDevice(int slave_id)
{
  for (auto& dev : devices)
  {
    if (slave_id == dev->slaveId())
      return dev;
  }
  return nullptr;
}

bool Modbus::poll(int tm)
{
  timeval tv = { 0, tm };
  modbus_set_response_timeout(_modbus, &tv);
  int recv_bytes = modbus_receive(_modbus, _query);
  if (recv_bytes == -1)
  {
    return false;
  }
  DeviceModbus* pdev = findDevice(_query[0]);
  if (!pdev)
  {
    LOG_ERROR("Unknown slave ID: %d", _query[0]);
    return false;
  }
  modbus_set_slave(_modbus, pdev->slaveId());
  switch (_query[modbus_get_header_length(_modbus)])
  {
  case MODBUS_READ_COILS:
  case MODBUS_READ_DISCRETE_INPUTS:
  case MODBUS_READ_INPUT_REGISTERS:
  case MODBUS_READ_HOLDING_REGISTERS:
    pdev->updateValues();
    modbus_reply(_modbus, _query, recv_bytes, pdev->modbusMap());
    break;

  case MODBUS_WRITE_SINGLE_COIL:
    LOG_INFO("MODBUS_WRITE_SINGLE_COIL");
    modbus_reply(_modbus, _query, recv_bytes, pdev->modbusMap());
    break;

  case MODBUS_WRITE_SINGLE_REGISTER:
    LOG_INFO("MODBUS_WRITE_SINGLE_REGISTER");
    modbus_reply(_modbus, _query, recv_bytes, pdev->modbusMap());
    break;

  case MODBUS_WRITE_MULTIPLE_COILS:
    LOG_INFO("MODBUS_WRITE_MULTIPLE_COILS");
    break;

  case MODBUS_WRITE_MULTIPLE_REGISTERS:
    LOG_INFO("MODBUS_WRITE_MULTIPLE_REGISTERS");
    break;

  default:
    LOG_ERROR("UNKNOWN FUNCTION");
    return false;
  }
  pdev->updateWidgets();
  return true;
}

bool ModbusPoll::addDevice(std::string& port_name, int baudrate, DeviceModbus* pdev)
{
  QMutexLocker ml(&_mutex);
  for (auto &mb : ModbusPoll::Instance()->modbus_list)
  {
    if (port_name == mb.portName())
    {
      if (baudrate != mb.baudrate())
        return false;
      mb.devices.push_back(pdev);
      return true;
    }
  }
  Modbus mb(port_name, baudrate);
  try
  {
    mb.open();
    mb.devices.push_back(pdev);
  }
  catch (md_exception& e)
  {
    return false;
  }
  
  ModbusPoll::Instance()->modbus_list.push_back(mb);
  return true;
}

void ModbusPoll::removeDevice(DeviceModbus* pdev)
{
  QMutexLocker ml(&_mutex);
  for (auto &mb : ModbusPoll::Instance()->modbus_list)
  {
    mb.devices.remove(pdev);
    if (mb.devices.size() == 0)
    {
      mb.close();
      ModbusPoll::Instance()->modbus_list.remove(mb);
    }
    break;
  }
}

void ModbusPoll::run()
{
  while (!_abort)
  {
    DWORD tm = GetTickCount();
    _mutex.lock();
    for (auto &kv : modbus_list)
    {
      while (kv.poll(10));
    }
    _mutex.unlock();
    DWORD tm2 = GetTickCount();
    Sleep(((tm2 - tm) < 10) ? (10 - (tm2 - tm)) : 1);
  }
}
