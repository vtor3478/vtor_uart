#ifndef MODBUS_H
#define MODBUS_H

#define MODBUS_CRC_BYTES (2)

class Modbus
{
public:
    Modbus();

    unsigned short crc16_modbus(unsigned char *pBuff, int len);
    bool pack_complete(unsigned char* data, unsigned int data_cnt);

    ~Modbus();

private:

};

#endif // MODBUS_H
