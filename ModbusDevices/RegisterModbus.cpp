#include "RegisterModbus.h"


RegisterModbus::RegisterModbus(QWidget *w) : _widget(w), _address_hi(-1), bit(-1)
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
    if (json["addr"].isArray())
    {
      _address = json["addr"].get((Json::UInt)0, Json::Value(0)).asUInt();
      _address_hi = json["addr"].get((Json::UInt)1, Json::Value(0)).asUInt();
    }
    else
      _address = json["addr"].asUInt();
    _format = json["fmt"].asString();

    if (json["bit"].isIntegral())
      bit = json["bit"].asUInt();

    if (_format.length())
    {
      if (!Value::parse(_format, &this->value))
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

