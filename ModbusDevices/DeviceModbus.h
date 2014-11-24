#pragma once
#include "Device.h"
#include "Register.h"
#include "libmodbus/modbus.h"
#include <QThread>

class DeviceModbus : public QThread, public Device
{
  Q_OBJECT

public:
  DeviceModbus(int sid);
  virtual ~DeviceModbus();

  virtual void switchOn(const char* port);
  virtual void switchOff();
  virtual bool load(const Json::Value& json);

  virtual const char* caption() { return (_caption = std::to_string(slave_id)).c_str(); }

  //virtual const std::vector<Register>& registers();

  Register* findRegister(ushort addr);
  Register* findRegister(const QWidget* widget);

protected:
  virtual void run();

private:
  DeviceWidget*     widget;
  bool              abort;
  modbus_t*         modbus;
  modbus_mapping_t* modbus_mapping;
  unsigned char     query[MODBUS_RTU_MAX_ADU_LENGTH];
  int               slave_id;
  std::string       _caption;

  std::vector<Register>     _registers;
};

