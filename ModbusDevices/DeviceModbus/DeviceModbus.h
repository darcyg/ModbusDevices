#pragma once
#include "Device.h"
#include "IncludeMe.h"
#include "RegisterModbus.h"
#include "ModbusControl.h"
#include <QThread>

class DeviceModbus : public Device
{
  //Q_OBJECT

public:
  DeviceModbus(int sid);
  virtual ~DeviceModbus();

  virtual void switchOn(const char* port, int baudrate);
  virtual void switchOff();
  virtual bool load(const Json::Value& json);
  virtual bool loadRegister(QWidget* w, const Json::Value& json);

  virtual const char* caption() { return (_caption = std::to_string(slave_id)).c_str(); }

  //RegisterModbus* findRegister(ushort addr);
  //RegisterModbus* findRegister(const QWidget* widget);

  int slaveId() const { return slave_id; }

  modbus_mapping_t* modbusMap() { return _modbus_mapping; }

  void updateValues();
  void updateWidgets();

protected:


private:
  bool              abort;
  modbus_mapping_t* _modbus_mapping;
  int               slave_id;
  std::string       _caption;

  std::vector<RegisterModbus>     _registers;
};

