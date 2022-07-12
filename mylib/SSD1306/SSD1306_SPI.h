#include "SSD1306.h"
#include "../MySPI.h"
#include "../MyPin.h"
#include "SSD1306_SPI_conf.h"

#define CSX_OUTPUT MYPIN_WRITE_MODE(CS_PIN)
#define CSX_HIGH MYPIN_WRITE_HIGH(CS_PIN)
#define CSX_LOW MYPIN_WRITE_LOW(CS_PIN)

#define DC_OUTPUT MYPIN_WRITE_MODE(DC_PIN)
#define CMD_MODE MYPIN_WRITE_LOW(DC_PIN)
#define DATA_MODE MYPIN_WRITE_HIGH(DC_PIN)

void _writeCmd(uint8_t cmd)
{
    CMD_MODE;
    CSX_LOW;
#ifdef DEBUG_LED
    pinMode(DEBUG_LED, OUTPUT);
    digitalWrite(DEBUG_LED, HIGH);
    delay(20);
#endif
    SPI_SEND8(cmd);
#ifdef DEBUG_LED
    digitalWrite(DEBUG_LED, LOW);
    delay(20);
#endif
    CSX_HIGH;
}

void _writeData8(uint8_t data)
{
    DATA_MODE;
    CSX_LOW;
    SPI_SEND8(data);
    CSX_HIGH;
}

void _INIT()
{
    CSX_OUTPUT;
    CSX_HIGH;

    DC_OUTPUT;
    CMD_MODE;
}

#define _BEFORE_DISPLAY

#define _START_DISPLAY 1

#define _DATA_MODE_DISPLAY \
    DATA_MODE;             \
    CSX_LOW

#define _SEND_DISPLAY(x) \
    _SET_SPI_REG(x);     \
    _WAIT_SPI_SEND;

#define _WAIT_SEND_DISPLAY

#define _STOP_DISPLAY \
    CSX_HIGH

#define _AFTER_DISPLAY