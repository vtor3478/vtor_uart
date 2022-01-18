#include "Modbus.h"

Modbus::Modbus()
{

}
unsigned short Modbus::crc16_modbus(unsigned char *pBuff, int len)
{
  if( len <= 0 )
  {
    return 0;
  }

  int   i, j;
  unsigned short crc = 0xffff;

  for(i = 0; i < len; i++)
  {
    crc ^= (unsigned short)( pBuff[i] );
    for(j = 0; j < 8; j++)
    {
      if(crc & 1)
      {
        crc >>= 1;
        crc  ^= 0xa001;
      }
      else
      {
        crc >>= 1;
      }
    }
  }
  return crc;
}

bool Modbus::pack_complete(unsigned char* data, unsigned int data_cnt)
{
  bool ret = true;
  // check crc correct
  unsigned short crc = crc16_modbus(data, data_cnt - MODBUS_CRC_BYTES);
  ret = ret && ( ((unsigned char)(crc) == data[data_cnt-2])
                  && ((unsigned char)(crc>>8) == data[data_cnt-1]) );

  return ret;
}



Modbus::~Modbus()
{

}

