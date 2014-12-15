#include "IOBoard.h"
#include "IncludeMe.h"

IOBoard::IOBoard()
{
}


IOBoard::~IOBoard()
{
}

bool IOBoard::HandleRequest(unsigned char *preq, int length, modbus_mapping_t* pmap)
{
  unsigned short reg = preq[2] << 8 | preq[3];
  unsigned short count = preq[4] << 8 | preq[5];
  switch (preq[1])
  {
  case MODBUS_READ_INPUT_REGISTERS:
    if (reg == 5 && count == 2)
    {
      pmap->tab_input_registers[5] = 0;
      pmap->tab_input_registers[6] = 0;
      for (int i = 0; i < 28; i++)
        pmap->tab_input_registers[5 + i / 16] |= pmap->tab_bits[i] ? (1 << i % 16) : 0;
      //LOG_INFO("READ INPUT: %04X%04X", MB_REVERSE_16(pmap->tab_input_registers[6]), MB_REVERSE_16(pmap->tab_input_registers[5]));
    }
    break;
  }
  return false;
}
