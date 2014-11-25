#pragma once
#include "Device.h"
#include "RegisterModbus.h"
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
  virtual bool loadRegister(QWidget* w, const Json::Value& json);

  virtual const char* caption() { return (_caption = std::to_string(slave_id)).c_str(); }

  //virtual const std::vector<Register>& registers();

  RegisterModbus* findRegister(ushort addr);
  RegisterModbus* findRegister(const QWidget* widget);

protected:
  virtual void run();

private:
  bool              abort;
  modbus_t*         modbus;
  modbus_mapping_t* modbus_mapping;
  unsigned char     query[MODBUS_RTU_MAX_ADU_LENGTH];
  int               slave_id;
  std::string       _caption;

  std::vector<RegisterModbus>     _registers;
};

