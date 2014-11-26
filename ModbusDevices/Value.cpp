#include "Value.h"
#include "IncludeMe.h"

Value::Value() :v_u32(0), type(TYPE_U32), fractional(0), divisor(1), multiplier(1)
{
}

Value::Value(Type t, int f, int m, int d) : v_u32(0), type(t), fractional(f), multiplier(m), divisor(d)
{
  if (!divisor)
    divisor = 1;
}

bool Value::parse(const std::string &sfmt, Value* pval)
{
  if (pval)
    pval->reset();
  auto lpos = sfmt.find('{');
  auto rpos = sfmt.find('}');
  if (lpos == std::string::npos || rpos == std::string::npos)
    return false;
  auto str = sfmt.substr(lpos + 1, rpos - lpos - 1);
  if (str.find('f') != std::string::npos)
  {
    if (pval)
      pval->type = Value::TYPE_FLOAT;
  }
  else
  {
    auto dot = str.find('.');
    if (dot == std::string::npos)
      return false;

    if (pval)
    {
      pval->type = (str.find('s') != std::string::npos) ? Value::TYPE_S32 : Value::TYPE_U32;
      pval->fractional = atoi(&str.c_str()[dot] + 1);
    }
  }
  if ((lpos = str.find('*')) != std::string::npos)
    pval->multiplier = atoi(&str.c_str()[lpos] + 1);
  if ((lpos = str.find(':')) != std::string::npos)
    pval->divisor = atoi(&str.c_str()[lpos] + 1);
  
  return true;
}

unsigned int Value::toUInt(int fract) const
{
  switch (type)
  {
  case TYPE_S32:
  case TYPE_U32:
    return (fract > fractional) ? v_u32 * std::pow(10, (fract - fractional)) : v_u32 / std::pow(10, (fractional - fract));
  case TYPE_FLOAT:
    if (fract > fractional)
      return v_float * std::pow(10, (fract - fractional)) + 0.5;
    else
      return v_u32 * std::pow(10, (fractional - fract)) + 0.5;
  }
  MD_THROW("Invalid type of Value");
  return 0;
}

int Value::toInt(int fract) const
{
  switch (type)
  {
  case TYPE_S32:
  case TYPE_U32:
    return (fract > fractional) ? v_u32 * std::pow(10, (fract - fractional)) : v_u32 * std::pow(10, (fractional - fract));
  case TYPE_FLOAT:
    if (fract > fractional)
      return v_float * std::pow(10, (fract - fractional)) + 0.5;
    else
      return v_u32 * std::pow(10, (fractional - fract)) + 0.5;
  }
  MD_THROW("Invalid type of Value");
  return 0;
}

float Value::toFloat() const
{
  switch (type)
  {
  case TYPE_S32:
  case TYPE_U32:
    return v_u32 / std::pow(10, (fractional));
  case TYPE_FLOAT:
    return v_float;
  }
  MD_THROW("Invalid type of Value");
  return 0;
}


