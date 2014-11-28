#ifndef DEVICE_H
#define DEVICE_H

#include "jsoncpp/json/value.h"
#include <QWidget>

class Device
{
public:
  virtual ~Device() {}
  virtual void switchOn(const char* port) = 0;
  virtual void switchOff() = 0;
  virtual void save(QString &key) { };
  virtual void load (QString &key) { };
  virtual bool load(const Json::Value& json) = 0;
  virtual bool loadRegister(QWidget* w, const Json::Value& json) = 0;

  virtual const char* caption() { return ""; }
  
};

#endif // DEVICE_H
