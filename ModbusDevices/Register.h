#pragma once

#include <QWidget>
#include <string>
#include "Value.h"

class Register
{
public:
  Register();
  virtual ~Register();

  bool loadJSON(std::string& jstr);

  int function() const { return _function;  }
  int address() const { return _address; }
  QWidget* widget() const { return _widget; }
  void setWidget(QWidget* w) { _widget = w; }
  //Value* value() { return &_value; }
  //void setValue(Value& v) { _value = v; }
  const std::string& format() const { return _format; }

  std::string toString() const;
  
  void valueToWidget();
  void widgetToValue();

  Value         value;

private:
  QWidget*      _widget;
  int           _function;
  int           _address;
  std::string   _format;
  int           _value;
};

