#pragma once

#include <QWidget>
#include <string>
#include "Value.h"
#include "jsoncpp/json/reader.h"

class Register
{
public:
  Register(QWidget *w);
  virtual ~Register();

  virtual bool setConfig(const Json::Value& json) = 0;

  QWidget* widget() const { return _widget; }
  void setWidget(QWidget* w) { _widget = w; }
  
  virtual std::string toString() const = 0;
  
  Value         value;

protected:
  QWidget*      _widget;
  
};

