#pragma once

#include "Device.h"
#include "IncludeMe.h"
#include <QThread>
#include <QtSerialPort/QSerialPort>

class DeviceTura : public QThread, public Device
{
public:
  DeviceTura();
  virtual ~DeviceTura();

  virtual void switchOn(const char* port);
  virtual void switchOff();
  virtual bool load(const Json::Value& json);
  virtual bool loadRegister(QWidget* w, const Json::Value& json);

  virtual const char* caption() { return "Tura"; }


  enum Code
  {
    ArchiveHour = 0x04,
    ArchiveDay = 0x05,
    CurrentData = 0x06,
    AccumulateData = 0x08,
    VersionConfig = 0x10,
    AdditionalConfig = 0x11,
    Journal = 0x24,
    StartSession = 0x80,
  };
protected:
  virtual void run();

private:
  QSerialPort     _serial;
  bool            _abort;
  unsigned char   _dev_name[9];
};

