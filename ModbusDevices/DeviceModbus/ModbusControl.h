#pragma once
#include "IncludeMe.h"
#include "libmodbus/modbus.h"
#include "DeviceModbus.h"
#include <list>
#include <memory>
#include <QThread>

class DeviceModbus;

class Modbus
{
public:
  Modbus(std::string &port, int baudrate);

  bool poll(int tm);

  DeviceModbus* findDevice(int slave_id);

  const std::string& portName() const { return _port_name; };
  const int baudrate() const { return _baudrate; }
  void open()
  {
    _modbus = modbus_new_rtu(_port_name.c_str(), _baudrate, 'N', 8, 1);
    if (!_modbus)
      MD_THROW("modbus_new_rtu failed");

    if (modbus_connect(_modbus) == -1)
    {
      modbus_free(_modbus);
      MD_THROW(std::string("Connection failed: ") + modbus_strerror(errno));
    }
  }
  void close()
  {
    modbus_close(_modbus);
    modbus_free(_modbus);
    _modbus = nullptr;
  }

  bool operator==(const Modbus& rhs) const
  {
    return _port_name == rhs.portName() && _baudrate == rhs.baudrate() && _modbus == rhs._modbus;
  }

  std::list<DeviceModbus*>    devices;

private:

  std::string                 _port_name;
  modbus_t*                   _modbus;
  int                         _baudrate;

  unsigned char     _query[MODBUS_RTU_MAX_ADU_LENGTH];
};

class ModbusPoll : public QThread, public Singleton<ModbusPoll>
{
  friend Singleton<ModbusPoll>;
  Q_OBJECT

protected:
  ModbusPoll();

  void run();

public:
  bool addDevice(std::string& port_name, int baudrate, DeviceModbus* pdev);
  void removeDevice(DeviceModbus* pdev);

  std::list<Modbus>  modbus_list;
  /*
  virtual void start()
  {
    if (_started)
      return;
    QThread::start();
  }
  */
private:
  QMutex  _mutex;
  bool    _abort;
  bool    _started;
};

