#include "Register.h"
#include "jsoncpp/json/reader.h"
#include "libmodbus/modbus.h"
#include <sstream>
#include <QLineEdit>
#include <QCheckBox>
#include "IncludeMe.h"

Register::Register() : _function(MODBUS_READ_HOLDING_REGISTERS), _address(0), _format("")
{
}


Register::~Register()
{
}

bool Register::loadJSON(std::string& jstr)
{
  Json::Value value;
  Json::Reader reader;
  if (!reader.parse(jstr, value))
  {
    LOG_ERROR("Parse failed\n%s", value.toStyledString().c_str());
    return false;
  }
  try
  {
    _function = value["func"].asUInt();
    _address = value["addr"].asUInt();
    _format = value["fmt"].asString();
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

std::string Register::toString() const
{
  std::stringstream ss;
  ss << "func: " << _function << ", addr: " << _address << ", fmt: " << _format;
  return ss.str();
}

void Register::valueToWidget()
{

}

void Register::widgetToValue()
{
  if (auto obj = qobject_cast<QLineEdit*>(_widget))
  {
    float val = obj->text().toFloat();
    value = obj->text().toFloat();
  }
  else if (auto obj = qobject_cast<QCheckBox*>(_widget))
  {
    value = obj->isChecked() ? 1 : 0;
  }
}