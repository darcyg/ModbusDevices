#pragma once

#include "IncludeMe.h"
#include <QWidget>
#include "Value.h"
#include "jsoncpp/json/reader.h"

class RegisterModbus
{
public:
  RegisterModbus(QWidget *w);
  virtual ~RegisterModbus();

  bool setConfig(const Json::Value& json);
  std::string toString() const;

  int function() const { return _function; }
  int address() const { return _address; }
  int address_hi() const { return _address_hi; }
  const std::string& format() const { return _format; }
  //Value& value() { return _value; }
  QWidget* widget() const { return _widget; }
  void setWidget(QWidget* w) { _widget = w; }

  int           bit;
  Value         value;

private:
  int           _function;
  int           _address;
  int           _address_hi;
  std::string   _format;
  QWidget*      _widget;
  
};

