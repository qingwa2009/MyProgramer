#include <Arduino.h>
#include "../MyTWI.h"
#include "DS1307.h"

#define DS1307_REG_SECONDS 0x00
#define DS1307_REG_MINUTES 0x01
#define DS1307_REG_HOURS 0x02
#define DS1307_REG_DAY 0x03
#define DS1307_REG_DATE 0x04
#define DS1307_REG_MONTH 0x05
#define DS1307_REG_YEAR 0x06
#define DS1307_REG_CONTROL 0x07
#define DS1307_REG_RAM 0x08

#define BCD2UINT(v) (((v) >> 4) * 10 + ((v)&0x0F))
#define UINT2BCD(v) ((((v) / 10) << 4) + ((v) % 10))

inline uint8_t hour_parse(uint8_t v)
{
    if (v & 0b01000000)
    {
        //12小时制
        if (v & 0b00100000)
        {
            //PM
            v = (v & 0x0F) + (((v >> 4) & 0x01) * 10) + 12;
        }
        else
        {
            //AM
            v &= 0x1F;
            v = BCD2UINT(v);
        }
    }
    else
    {
        //24小时制
        v &= 0x3F;
        v = BCD2UINT(v);
    }
    return v;
}

inline uint8_t _read(uint8_t addr)
{

    uint8_t v = 0xFF;
    if (TWIMasterSyncStart(DS1307_I2C_ADDR, TWI_WRITE_MODE))
    {
        TWIMasterSyncWriteByteWaitAck(addr);
        TWIMasterSyncStart(DS1307_I2C_ADDR, TWI_READ_MODE);
        TWIMasterSyncReadByteWaitAck(&v, 1);
        TWIMasterSyncStop();
    }
    return v;
}

inline void _write(uint8_t addr, uint8_t data)
{
    if (TWIMasterSyncStart(DS1307_I2C_ADDR, TWI_WRITE_MODE))
    {
        TWIMasterSyncWriteByteWaitAck(addr);
        TWIMasterSyncWriteByteWaitAck(data);
        TWIMasterSyncStop();
    }
}

uint8_t getSecond()
{
    uint8_t ret = _read(DS1307_REG_SECONDS);
    ret &= 0x7F;
    return BCD2UINT(ret);
}

uint8_t getMinute()
{
    uint8_t ret = _read(DS1307_REG_MINUTES);
    return BCD2UINT(ret);
}

/*00-24*/
uint8_t getHour()
{
    uint8_t ret = _read(DS1307_REG_HOURS);
    return hour_parse(ret);
}

/*month day 1-31*/
uint8_t getDate()
{
    uint8_t ret = _read(DS1307_REG_DATE);
    return BCD2UINT(ret);
}

/*weekday 1-7*/
uint8_t getDay()
{
    uint8_t ret = _read(DS1307_REG_DAY);
    return ret;
}

uint8_t getMonth()
{
    uint8_t ret = _read(DS1307_REG_MONTH);
    return BCD2UINT(ret);
}

/*YY: 00-99*/
uint8_t getYear()
{
    uint8_t ret = _read(DS1307_REG_YEAR);
    return BCD2UINT(ret);
}

bool getDateTime(DateTime *dt)
{
    if (TWIMasterSyncStart(DS1307_I2C_ADDR, TWI_WRITE_MODE))
    {
        TWIMasterSyncWriteByteWaitAck(DS1307_REG_SECONDS);
        TWIMasterSyncStart(DS1307_I2C_ADDR, TWI_READ_MODE);
        TWIMasterSyncRead((uint8_t *)dt, sizeof(DateTime));
        TWIMasterSyncStop();
    }
    else
    {
        memset(dt, 0xFF, sizeof(DateTime));
        return 0;
    }

    dt->second &= 0x7F;
    dt->second = BCD2UINT(dt->second);
    dt->minute = BCD2UINT(dt->minute);
    dt->hour = hour_parse(dt->hour);
    dt->day = dt->day;
    dt->date = BCD2UINT(dt->date);
    dt->month = BCD2UINT(dt->month);
    dt->year = BCD2UINT(dt->year);
    return 1;
}

bool setDateTime(DateTime dt)
{
    if (TWIMasterSyncStart(DS1307_I2C_ADDR, TWI_WRITE_MODE))
    {
        TWIMasterSyncWriteByteWaitAck(0);
        TWIMasterSyncWriteByteWaitAck(dt.second > 59 ? UINT2BCD(59) : UINT2BCD(dt.second));
        TWIMasterSyncWriteByteWaitAck(dt.minute > 59 ? UINT2BCD(59) : UINT2BCD(dt.minute));
        TWIMasterSyncWriteByteWaitAck(dt.hour > 23 ? UINT2BCD(23) : UINT2BCD(dt.hour));
        TWIMasterSyncWriteByteWaitAck(0);
        TWIMasterSyncWriteByteWaitAck(dt.date > 31 ? UINT2BCD(31) : UINT2BCD(dt.date));
        TWIMasterSyncWriteByteWaitAck(dt.month > 12 ? UINT2BCD(12) : UINT2BCD(dt.month));
        TWIMasterSyncWriteByteWaitAck(dt.year > 99 ? UINT2BCD(99) : UINT2BCD(dt.year));
        TWIMasterSyncWriteByteWaitAck(0);
        TWIMasterSyncStop();
        return 1;
    }
    else
    {
        return 0;
    }
}

void initDS1307()
{
    DS1307Resume();
    setClockOutMode(DS1307_CLOCKOUT_LOW);
}

void DS1307Stop()
{
    uint8_t v = _read(0) | 0x80;
    _write(0, v);
}

void DS1307Resume()
{
    uint8_t v = _read(0) & (~0x80);
    _write(0, v);
}

bool DS1307IsRunning()
{
    return !(_read(0) & 0x80);
}

uint8_t getClockOutMode()
{
    return _read(DS1307_REG_CONTROL);
}

void setClockOutMode(uint8_t clockoutMode)
{
    _write(DS1307_REG_CONTROL, clockoutMode);
}

/*addr: 0x08~0x3F, 共56字节*/
void DS1307WriteRAM(uint8_t *data, uint8_t len, uint8_t addr)
{
    if (TWIMasterSyncStart(DS1307_I2C_ADDR, TWI_WRITE_MODE))
    {
        TWIMasterSyncWriteByteWaitAck(addr);
        TWIMasterSyncWrite(data, len);
        TWIMasterSyncStop();
    }
}
/*addr: 0x08~0x3F, 共56字节*/
void DS13107ReadRAM(uint8_t *buf, uint8_t len, uint8_t addr)
{
    if (TWIMasterSyncStart(DS1307_I2C_ADDR, TWI_WRITE_MODE))
    {
        TWIMasterSyncWriteByteWaitAck(addr);
        TWIMasterSyncStart(DS1307_I2C_ADDR, TWI_READ_MODE);
        TWIMasterSyncRead(buf, len);
        TWIMasterSyncStop();
    }
}