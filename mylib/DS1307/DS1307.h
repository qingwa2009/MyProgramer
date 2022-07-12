#ifndef __DS1307_H
#define __DS1307_H

#define DS1307_I2C_ADDR 0x68
#define DS1307_I2C_FREQUENCY 100000
#define DS1307_MAX_BUFFER_SIZE 64
#define DS1307_RAM_START_ADDR 0x08

#define DS1307_CLOCKOUT_LOW 0x00
#define DS1307_CLOCKOUT_HIGH 0x80
#define DS1307_CLOCKOUT_1Hz 0x10
#define DS1307_CLOCKOUT_4kHz 0x11
#define DS1307_CLOCKOUT_8kHz 0x12
#define DS1307_CLOCKOUT_32kHz 0x13

typedef struct
{
    uint8_t second; /*0~59*/
    uint8_t minute; /*0~59*/
    uint8_t hour;   /*0~23*/
    uint8_t day;    /*1-7*/
    uint8_t date;   /*1-31*/
    uint8_t month;  /*1~12*/
    uint8_t year;   /*00~99*/
} DateTime;

#ifdef __cplusplus
extern "C"
{
#endif
    void initDS1307();
    bool DS1307IsRunning();
    void DS1307Stop();
    void DS1307Resume();
    uint8_t getSecond();
    uint8_t getYear();
    uint8_t getMonth();
    uint8_t getDay();
    uint8_t getDate();
    uint8_t getHour();
    uint8_t getMinute();
    bool getDateTime(DateTime *dt);
    bool setDateTime(DateTime dt);
    uint8_t getClockOutMode();
    void setClockOutMode(uint8_t clockoutMode);
    void DS1307WriteRAM(uint8_t *data, uint8_t len, uint8_t addr);
    void DS13107ReadRAM(uint8_t *buf, uint8_t len, uint8_t addr);
#ifdef __cplusplus
}
#endif

#endif