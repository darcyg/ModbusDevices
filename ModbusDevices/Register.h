#pragma once

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
  const std::string& format() const { return _format; }

private:
  int           _function;
  int           _address;
  std::string   _format;
};

