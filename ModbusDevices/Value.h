#pragma once
#include <string>

struct Value
{
  enum Type
  {
    TYPE_U32 = 0,
    TYPE_S32 = 1,
    TYPE_FLOAT = 2,
  };

  Value();
  Value(Type t, int f, int m = 1, int d = 1);

  Value& operator=(int v)
  {
    if (type == TYPE_FLOAT)
      v_float = (float)v;
    else
      v_s32 = (int)(v / std::pow(10, fractional) + 0.5);
    return *this;
  }

  Value& operator=(float v)
  {
    if (type == TYPE_FLOAT)
      v_float = v;
    else
      v_u32 = (unsigned int)(v * std::pow(10, fractional) + 0.5);
    return *this;
  }

  bool operator==(const Value& rval) const
  {
    return (v_u32 == rval.v_u32 && type == rval.type && fractional == rval.fractional &&
            multiplier == rval.multiplier && divisor == rval.divisor);
  }

  union
  {
    unsigned int  v_u32;
    int           v_s32;
    float         v_float;
  };

  Type type;
  int fractional;
  int multiplier;
  int divisor;
  

  void reset()
  {
    type = TYPE_U32;
    v_u32 = 0;
    fractional = 0;
    multiplier = 1;
    divisor = 1;
  }
  static bool parse(const std::string &sfmt, Value* pval);
};
