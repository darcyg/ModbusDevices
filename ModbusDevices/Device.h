#ifndef DEVICE_H
#define DEVICE_H

#include "jsoncpp/json/value.h"
#include <QWidget>

class Device
{
public:
  virtual ~Device() {}
  virtual void switchOn(const char* port, int baudrate) = 0;
  virtual void switchOff() = 0;
  virtual void saveState(QString &key) { };
  virtual void loadState(QString &key) { };
  virtual bool load(const Json::Value& json, const QString& fname) = 0;
  virtual bool loadRegister(QWidget* w, const Json::Value& json) = 0;

  virtual const char* caption() { return ""; }

};

#endif // DEVICE_H
