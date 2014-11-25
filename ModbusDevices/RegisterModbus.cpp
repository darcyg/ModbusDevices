#include "RegisterModbus.h"


RegisterModbus::RegisterModbus(QWidget *w) : _widget(w)
{
}


RegisterModbus::~RegisterModbus()
{
}


bool RegisterModbus::setConfig(const Json::Value& json)
{
  try
  {
    _function = json["func"].asUInt();
    _address = json["addr"].asUInt();
    _format = json["fmt"].asString();
    if (_format.length())
    {
      if (!Value::parse(_format, &this->_value))
        LOG_ERROR("Format parsing failed");
    }
  }
  catch (std::runtime_error& e)
  {
    LOG_ERROR(e.what());
  }

  return true;
}

std::string RegisterModbus::toString() const
{
  std::stringstream ss;
  ss << "func: " << _function << ", addr: " << _address << ", fmt: " << _format;
  return ss.str();
}

