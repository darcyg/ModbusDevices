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
  const std::string& format() const { return _format; }
  const Value& value() const { return _value; }
  QWidget* widget() const { return _widget; }
  void setWidget(QWidget* w) { _widget = w; }

private:
  Value         _value;
  int           _function;
  int           _address;
  std::string   _format;
  QWidget*      _widget;
};

