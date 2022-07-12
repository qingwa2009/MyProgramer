#include "MySPI.h"
/*
DDB5:SCK
DDB4:MISO
DDB3:MOSI
DDB2:~SS use in slave mode only
*/
#define MOSI_OUTPUT (DDRB |= _BV(DDB3))
#define MOSI_INPUT (DDRB &= ~(_BV(DDB3)))
#define MISO_OUTPUT (DDRB |= _BV(DDB4))
#define MISO_INPUT (DDRB &= ~(_BV(DDB4)))
#define SCK_OUTPUT (DDRB |= _BV(DDB5))
#define SCK_INPUT (DDRB &= ~(_BV(DDB5)))
#define SS_OUTPUT (DDRB |= _BV(DDB2))
#define SS_INPUT (DDRB &= ~(_BV(DDB2)))

/*  SCK_OUTPUT;
    MOSI_OUTPUT;
    MISO_INPUT;
    SS_OUTPUT; SS会设成OUTPUT，不然会自动切换到从机模式；
    datasheet: If SS is configured as an input and is driven low while MSTR is set, 
    MSTR will be cleared, and SPIF in SPSR will become set
*/
void initSPIMaster(enum MySPI_Fosc Fosc)
{
    SCK_OUTPUT;
    MOSI_OUTPUT;
    MISO_INPUT;
    SS_OUTPUT;

    SPCR = _BV(MSTR);
    setFrequency(Fosc);
    SPCR |= _BV(SPE);
}

/*
    MISO_INPUT;
    SCK_INPUT;
    MOSI_OUTPUT;
    SS_INPUT;
*/
void initSPISlave()
{
    MISO_INPUT;
    SCK_INPUT;
    MOSI_OUTPUT;
    SS_INPUT;
    SPCR = _BV(SPIE) | _BV(SPE);
}

void setFrequency(enum MySPI_Fosc Fosc)
{
    switch (Fosc)
    {
    case MySPI_Fosc_1_2:
        SPSR |= _BV(SPI2X); //双倍速
        SPCR &= ~(_BV(SPR1));
        SPCR &= ~(_BV(SPR0));
        break;
    case MySPI_Fosc_1_4:
        SPSR &= ~(_BV(SPI2X));
        SPCR &= ~(_BV(SPR1));
        SPCR &= ~(_BV(SPR0));
        break;
    case MySPI_Fosc_1_8:
        SPSR |= _BV(SPI2X); //双倍速
        SPCR &= ~(_BV(SPR1));
        SPCR |= (_BV(SPR0));
        break;
    case MySPI_Fosc_1_16:
        SPSR &= ~(_BV(SPI2X));
        SPCR &= ~(_BV(SPR1));
        SPCR |= (_BV(SPR0));
        break;
    case MySPI_Fosc_1_32:
        SPSR |= _BV(SPI2X); //双倍速
        SPCR |= (_BV(SPR1));
        SPCR &= ~(_BV(SPR0));
        break;
    case MySPI_Fosc_1_64:
        SPSR &= ~(_BV(SPI2X));
        SPCR |= (_BV(SPR1));
        SPCR &= ~(_BV(SPR0));
        break;
    default:
        SPSR &= ~(_BV(SPI2X));
        SPCR |= (_BV(SPR1));
        SPCR |= (_BV(SPR0));
        break;
    }
}

/*设置传输顺序，默认先传输最高位*/
void setDataOrder(bool isMSB)
{
    if (isMSB)
    {
        SPCR &= ~(_BV(DORD));
    }
    else
    {
        SPCR |= _BV(DORD);
    }
}
/*设置时钟空闲时处于高低电平，默认低电平*/
void setSCKIdelLevel(bool isLow)
{
    if (isLow)
    {
        SPCR &= ~(_BV(CPOL));
    }
    else
    {
        SPCR |= (_BV(CPOL));
    }
}
/*设置第一还是第二个时钟边缘发送数据，默认第一个时钟边缘*/
void setEdgeToSendData(bool isFirstEdge)
{
    if (isFirstEdge)
    {
        SPCR &= ~(_BV(CPHA));
    }
    else
    {
        SPCR |= (_BV(CPHA));
    }
}

void SPISend(uint8_t *s, uint16_t len)
{
    _SET_SPI_REG(*s++);
    while (--len)
    {
        _WAIT_SPI_SEND;
        _SET_SPI_REG(*s++);
    }
    _WAIT_SPI_SEND;
}

void SPIRead(uint8_t *s, uint16_t len)
{
    // while (len--)
    // {
    //     *s = SPI_READ8();
    //     s++;
    // }
    _SET_SPI_REG(0);
    while (--len)
    {
        _WAIT_SPI_SEND;
        *s = _GET_SPI_REG();
        _SET_SPI_REG(0);
        s++;
    }
    _WAIT_SPI_SEND;
    *s = _GET_SPI_REG();
}

// ISR(SPI_STC_vect)
// {
// }