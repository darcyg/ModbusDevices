#pragma once

#include <QWidget>
#include <string>
#include "LoggerWidget.h"

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
  const std::string& format() const { return _format; }

private:
  QWidget*      _widget;
  int           _function;
  int           _address;
  std::string   _format;
  int           _value;
};

