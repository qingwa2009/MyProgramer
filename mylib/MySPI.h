#ifndef _MY_SPI_H
#define _MY_SPI_H
#include <Arduino.h>

enum MySPI_Fosc
{
    MySPI_Fosc_1_2,
    MySPI_Fosc_1_4,
    MySPI_Fosc_1_8,
    MySPI_Fosc_1_16,
    MySPI_Fosc_1_32,
    MySPI_Fosc_1_64,
    MySPI_Fosc_1_128
};

// typedef unsigned char uint8_t;
// typedef unsigned int uint16_t;

#define _SET_SPI_REG(c) \
    SPDR = (c)

#define _GET_SPI_REG() \
    SPDR

#define _WAIT_SPI_SEND \
    while (!(SPSR & _BV(SPIF)))

#define SPI_SEND8(c)     \
    do                   \
    {                    \
        _SET_SPI_REG(c); \
        _WAIT_SPI_SEND;  \
    } while (0)

#define SPI_SEND16(i)        \
    do                       \
    {                        \
        SPI_SEND8((i) >> 8); \
        SPI_SEND8((i));      \
    } while (0)

#define SPI_READ8()     \
    ({                  \
        SPI_SEND8(0);   \
        _GET_SPI_REG(); \
    })

#ifdef __cplusplus
extern "C"
{
#endif
    void initSPIMaster(enum MySPI_Fosc Fosc);
    void initSPISlave();
    void setFrequency(enum MySPI_Fosc Fosc);
    void setDataOrder(bool isMSB);
    void setSCKIdelLevel(bool isLow);
    void setEdgeToSendData(bool isFirstEdge);
    void SPISend(uint8_t *s, uint16_t len);
    void SPIRead(uint8_t *s, uint16_t len);
#ifdef __cplusplus
}
#endif

#endif