#ifndef DEVICE_H
#define DEVICE_H

#include "DeviceWidget.h"
#include "Register.h"
#include "jsoncpp/json/value.h"


class Device
{
public:
  virtual void switchOn(const char* port) = 0;
  virtual void switchOff() = 0;
  virtual bool load(const Json::Value& json) = 0;

  //virtual const std::vector<Register>& registers() = 0;

  virtual const char* caption() { return ""; }
  
};

#endif // DEVICE_H
