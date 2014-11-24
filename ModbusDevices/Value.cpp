#include "Value.h"

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