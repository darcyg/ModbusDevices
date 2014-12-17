#pragma once

#include "IncludeMe.h"
#include "libmodbus/modbus.h"
#include "DeviceModbus.h"
#include <map>
#include <memory>

struct Modbus
{
  modbus_t*                 _modbus;
  std::list<DeviceModbus*>  _devices;
};

class SerialModbus: QObject
{

  Q_OBJECT

  SerialModbus();
public:
  
  ~SerialModbus();

  typedef void(*fn_dataReceived)(const char* data, int size);

public slots:
  void readData();
  void poll();

private:
  std::string       _name;
  int               _baudrate;
  std::list<Modbus> _modbus_list;
  std::list<fn_dataReceived> _dataReceived;

public:
  static std::shared_ptr<SerialModbus> open(const char* name, int baudrate, fn_dataReceived cb);
  static void close(std::shared_ptr<SerialModbus> port);
  

private:
  static std::map<const char*, std::shared_ptr<SerialModbus>> _ports;
  static SerialModbus *_dummy;
};

