#include "Register.h"
#include "jsoncpp/json/reader.h"

Register::Register() : _function(0), _address(0), _format("")
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
  }
  catch (std::runtime_error& e)
  {
    LOG_ERROR(e.what());
  }
  
  return true;
}
