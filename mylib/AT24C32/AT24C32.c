#include <Arduino.h>
#include "../MyTWI.h"
#include "AT24C32.h"

void AT24C32_WriteByte(uint8_t data, uint16_t addr, uint8_t A2A1A0)
{
    while (!TWIMasterSyncStart(AT24C32_I2C_ADDR + A2A1A0, TWI_WRITE_MODE))
        ;
    TWIMasterSyncWriteByteWaitAck(addr >> 8);
    TWIMasterSyncWriteByteWaitAck(addr);
    TWIMasterSyncWriteByteWaitAck(data);
    TWIMasterSyncStop();
}

/*1 page 32 bytes，addr的低5位会自动递增超过将回到0并覆盖前面的数据*/
/*proteus的AT24C32A 1 page 16 bytes迷*/
void AT24C32_WritePage(uint8_t *data, uint8_t len, uint16_t addr, uint8_t A2A1A0)
{
    while (!TWIMasterSyncStart(AT24C32_I2C_ADDR + A2A1A0, TWI_WRITE_MODE))
        ;
    TWIMasterSyncWriteByteWaitAck(addr >> 8);
    TWIMasterSyncWriteByteWaitAck(addr);
    TWIMasterSyncWrite(data, len);
    TWIMasterSyncStop();
}

uint8_t AT24C32_ReadByte(uint16_t addr, uint8_t A2A1A0)
{
    while (!TWIMasterSyncStart(AT24C32_I2C_ADDR + A2A1A0, TWI_WRITE_MODE))
        ;
    TWIMasterSyncWriteByteWaitAck(addr >> 8);
    TWIMasterSyncWriteByteWaitAck(addr);
    TWIMasterSyncStart(AT24C32_I2C_ADDR + A2A1A0, TWI_READ_MODE);
    uint8_t data;
    TWIMasterSyncReadByteWaitAck(&data, 1);
    TWIMasterSyncStop();
    return data;
}

void At24C32_ReadBytes(uint8_t *buf, uint16_t len, uint16_t addr, uint8_t A2A1A0)
{
    while (!TWIMasterSyncStart(AT24C32_I2C_ADDR + A2A1A0, TWI_WRITE_MODE))
        ;
    TWIMasterSyncWriteByteWaitAck(addr >> 8);
    TWIMasterSyncWriteByteWaitAck(addr);
    TWIMasterSyncStart(AT24C32_I2C_ADDR + A2A1A0, TWI_READ_MODE);
    TWIMasterSyncRead(buf, len);
    TWIMasterSyncStop();
}