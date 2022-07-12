#ifndef __AT24C32_H
#define __AT24C32_H
#include <Arduino.h>

/*需要加上A2 A1 A0引脚的值*/
#define AT24C32_I2C_ADDR 0x50

#ifdef __cplusplus
extern "C"
{
#endif
    void AT24C32_WriteByte(uint8_t data, uint16_t addr, uint8_t A2A1A0);
    void AT24C32_WritePage(uint8_t *data, uint8_t len, uint16_t addr, uint8_t A2A1A0);
    uint8_t AT24C32_ReadByte(uint16_t addr, uint8_t A2A1A0);
    void At24C32_ReadBytes(uint8_t *buf, uint16_t len, uint16_t addr, uint8_t A2A1A0);
#ifdef __cplusplus
}
#endif

#endif