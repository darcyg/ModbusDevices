#pragma once
#include "libmodbus/modbus.h"

class IOBoard
{
public:
  IOBoard();
  virtual ~IOBoard();

  bool HandleRequest(unsigned char *preq, int length, modbus_mapping_t* pmap);
};

