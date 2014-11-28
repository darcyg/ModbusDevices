#pragma once

#include "Device.h"
#include "IncludeMe.h"
#include "Value.h"
#include <QThread>
#include <QLabel>
#include <QtSerialPort/QSerialPort>
#include "windows.h"
#include "Serial/Serial.h"

struct RegisterTura
{
  RegisterTura() : widget(0), value(Value::TYPE_FLOAT) {}
  QWidget*    widget;
  Value       value;
};

class DeviceTura : public QThread, public Device
{
public:
  DeviceTura();
  virtual ~DeviceTura();

  virtual void switchOn(const char* port);
  virtual void switchOff();
  virtual bool load(const Json::Value& json);
  virtual bool loadRegister(QWidget* w, const Json::Value& json);
  virtual void save(QString &key);
  virtual void load(QString &key);

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
  void processSession();
  int serialRead(char *pbuf, int sz, unsigned int tm);
  int serialWrite(char *pbuf, int sz);
  void updateDisplaInfo();

private:
  CSerial*        _cserial;
  QString         _portName;
  bool            _abort;
  unsigned char   _dev_name[9];
  QLabel*         _info_widget;

  RegisterTura    _current_fr[3];
  RegisterTura    _accum_fr[3];

  struct
  {
    int   tx_bytes;
    int   rx_bytes;
    int   sessions;
  } _io_stat;
};

