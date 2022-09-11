#include "myATmega328.h"
#include "../mylib/MySPI.h"
#include "../mylib/MyPin.h"
#include "../mylib/MyPWM.h"

// #define MOSI 11
// #define MISO 12
// #define SCK 13
#define PIN_RST 2
#define PIN_XTAL1 3 //3号引脚输出8M时钟信号

void sendCmd(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
    SPI_SEND8(a);
    SPI_SEND8(b);
    SPI_SEND8(c);
    SPI_SEND8(d);
}

void enableXTAL1CLK()
{
    MYPIN_WRITE_MODE(3);
    timer2SetClockSource(Timer2_Clock_1);
    timer2SetCountType(Timer2_Count_CTC_0_OCRA);
    timer2SetCountValue(0);
    timer2SetAOnCompMatchOutType(Timer2_OCx_Disconnected);
    timer2SetACompMatchValue(0);
    timer2SetBOnCompMatchOutType(Timer2_OCx_NonPWM_Toggle);
    timer2SetBCompMatchValue(0);
}

void disalbeXTAL1CLK()
{
    timer2SetBOnCompMatchOutType(Timer2_OCx_Disconnected);
    MYPIN_WRITE_LOW(3);
    MYPIN_READ_MODE(3);
}

bool enableProgram()
{
    uint8_t ret;
    for (uint8_t i = 0; i < 16; i++)
    {
        delay(50);
        SPI_SEND8(0xAC);
        SPI_SEND8(0x53);
        SPI_SEND8(0x00);
        ret = _GET_SPI_REG();
        SPI_SEND8(0x00);
        if (ret == 0x53)
        {
            return 1;
        }
        MYPIN_WRITE_HIGH(PIN_RST);
        delay(1);
        MYPIN_WRITE_LOW(PIN_RST);
    }
    return 0;
}

bool isFlashBusy()
{
    sendCmd(0xF0, 0x00, 0x00, 0x00);
    return !(_GET_SPI_REG());
}

bool waitForBusy()
{
    for (uint16_t i = 0; i < 1024; i++)
    {
        if (!isFlashBusy()) /*好像每什么效果*/
            return 1;
    }
    return 0;
}

/*
Lock Bit    Bit Description     Default Value
            7   –               1 (unprogrammed)
            6   –               1 (unprogrammed)
BLB12       5   Boot lock bit   1 (unprogrammed)
BLB11       4   Boot lock bit   1 (unprogrammed)
BLB02       3   Boot lock bit   1 (unprogrammed)
BLB01       2   Boot lock bit   1 (unprogrammed)
LB2         1   Lock bit        1 (unprogrammed)
LB1         0   Lock bit        1 (unprogrammed)
*/
uint8_t readLockBits()
{
    sendCmd(0x58, 0, 0, 0);
    return _GET_SPI_REG();
}

/*
Lock Bit    Bit Description     Default Value
            7   –               1 (unprogrammed)
            6   –               1 (unprogrammed)
BLB12       5   Boot lock bit   1 (unprogrammed)
BLB11       4   Boot lock bit   1 (unprogrammed)
BLB02       3   Boot lock bit   1 (unprogrammed)
BLB01       2   Boot lock bit   1 (unprogrammed)
LB2         1   Lock bit        1 (unprogrammed)
LB1         0   Lock bit        1 (unprogrammed)
*/
bool writeLockBits(uint8_t data)
{
    sendCmd(0xAC, 0xE0, 0, data);
    delay(5 * 2);
    return waitForBusy();
}

/*
0x000   0x001   0x002
0x1E    0x95    0x0F
*/
uint8_t readSignatureByte(uint8_t a)
{
    sendCmd(0x30, 0, a, 0);
    return _GET_SPI_REG();
}

/*  
Fuse Byte   Bit Description             Default Value
CKDIV8      7   Divide clock by 8       0 (programmed)
CKOUT       6   Clock output            1 (unprogrammed)
SUT1        5   Select start-up time    1 (unprogrammed)
SUT0        4   Select start-up time    0 (programmed)
CKSEL3      3   Select clock source     0 (programmed)
CKSEL2      2   Select clock source     0 (programmed)
CKSEL1      1   Select clock source     1 (unprogrammed)
CKSEL0      0   Select clock source     0 (programmed)
*/
uint8_t readFuseLowBits()
{
    sendCmd(0x50, 0, 0, 0);
    return _GET_SPI_REG();
}

/*  
arduino uno 16m 时钟信号可以设置为 0xFF 
(Low Power Crystal Oscillator Clock Selection)
(Crystal oscillator, slowly rising power)
Fuse Byte   Bit Description             Default Value
CKDIV8      7   Divide clock by 8       0 (programmed)
CKOUT       6   Clock output            1 (unprogrammed)
SUT1        5   Select start-up time    1 (unprogrammed)
SUT0        4   Select start-up time    0 (programmed)
CKSEL3      3   Select clock source     0 (programmed)
CKSEL2      2   Select clock source     0 (programmed)
CKSEL1      1   Select clock source     1 (unprogrammed)
CKSEL0      0   Select clock source     0 (programmed)
*/
bool writeFuseLowBits(uint8_t data)
{
    sendCmd(0xAC, 0xA0, 0, data);
    delay(5 * 2);
    return waitForBusy();
}

/*
Fuse Byte   Bit Description                                         Default Value
RSTDISBL    7   External reset disable                              1 (unprogrammed)
DWEN        6   debugWIRE enable                                    1 (unprogrammed)
SPIEN       5   Enable serial program and data downloading          0 (programmed, SPI programming enabled)
WDTON       4   Watchdog timer always On                            1 (unprogrammed)
EESAVE      3   EEPROM memory is preserved through the chip erase   1 (unprogrammed), EEPROM not reserved
BOOTSZ1     2   Select boot size                                    0 (programmed)
BOOTSZ0     1   Select boot size                                    0 (programmed)(4)
BOOTRST     0   Select reset vector                                 1 (unprogrammed)
*/
uint8_t readFuseHighBits()
{
    sendCmd(0x58, 0x08, 0, 0);
    return _GET_SPI_REG();
}

/*
Fuse Byte   Bit Description                                         Default Value
RSTDISBL    7   External reset disable                              1 (unprogrammed)
DWEN        6   debugWIRE enable                                    1 (unprogrammed)
SPIEN       5   Enable serial program and data downloading          0 (programmed, SPI programming enabled)
WDTON       4   Watchdog timer always On                            1 (unprogrammed)
EESAVE      3   EEPROM memory is preserved through the chip erase   1 (unprogrammed), EEPROM not reserved
BOOTSZ1     2   Select boot size                                    0 (programmed)
BOOTSZ0     1   Select boot size                                    0 (programmed)(4)
BOOTRST     0   Select reset vector                                 1 (unprogrammed)
*/
bool writeFuseHighBits(uint8_t data)
{
    sendCmd(0xAC, 0xA8, 0, data);
    delay(5 * 2);
    return waitForBusy();
}

/*
Fuse Byte   Bit Description                         Default Value
BODLEVEL2(1) 2  Brown-out detector trigger level    1 (unprogrammed)
BODLEVEL1(1) 1  Brown-out detector trigger level    1 (unprogrammed)
BODLEVEL0(1) 0  Brown-out detector trigger level    1 (unprogrammed)
*/
uint8_t readFuseExtBits()
{
    sendCmd(0x50, 0x08, 0, 0);
    return _GET_SPI_REG();
}

/*
Fuse Byte   Bit Description                         Default Value
BODLEVEL2(1) 2  Brown-out detector trigger level    1 (unprogrammed)
BODLEVEL1(1) 1  Brown-out detector trigger level    1 (unprogrammed)
BODLEVEL0(1) 0  Brown-out detector trigger level    1 (unprogrammed)
*/
bool writeFuseExtBits(uint8_t data)
{
    sendCmd(0xAC, 0xA4, 0, data);
    delay(5 * 2);
    return waitForBusy();
}

uint8_t readCalibrationByte()
{
    sendCmd(0x38, 0x00, 0, 0);
    return _GET_SPI_REG();
}

void loadExtendedAddrByte(uint8_t addr)
{
    sendCmd(0x4D, 0x00, addr, 0);
}

/*
-------------------FLASH 32kbytes--------------------
256 pages 128bytes/page 64words/page
PCPAGE PC[13:6]
PCWORD PC[5:0]
*/
uint8_t readProgramMemoryLowByte(uint16_t addr)
{
    SPI_SEND8(0x20);
    SPI_SEND8(addr >> 8);
    SPI_SEND8(addr);
    SPI_SEND8(0x00);
    return _GET_SPI_REG();
}

/*
-------------------FLASH 32kbytes--------------------
256 pages 128bytes/page 64words/page
PCPAGE PC[13:6]
PCWORD PC[5:0]
*/
uint8_t readProgramMemoryHighByte(uint16_t addr)
{
    SPI_SEND8(0x28);
    SPI_SEND8(addr >> 8);
    SPI_SEND8(addr);
    SPI_SEND8(0x00);
    return _GET_SPI_REG();
}

// /*addr: PC[5:0] max: 3F*/
// void loadProgramMemPageOneWord(uint8_t addr, uint8_t lByte, uint8_t hByte)
// {
//     sendCmd(0x40, 0x00, addr, lByte);
//     sendCmd(0x48, 0x00, addr, hByte);
// }

// /*addr: PC[5:0] max: 3F*/
// void loadProgramMemPageLowByte(uint8_t addr, uint8_t lByte)
// {
//     sendCmd(0x40, 0x00, addr, lByte);
// }
// /*addr: PC[5:0] max: 3F
//   note: must call loadProgramMemPageLowByte first
//   */
// void loadProgramMemPageHighByte(uint8_t addr, uint8_t hByte)
// {
//     sendCmd(0x48, 0x00, addr, hByte);
// }

// /*Note: len of bs must be 128*/
// void loadProgramMemPage(uint8_t *bs)
// {
//     for (uint8_t i = 0, j = 0; i < SIZE_BYTES_PER_PAGE; i += 2, j++)
//     {
//         loadProgramMemPageOneWord(j, bs[i], bs[i + 1]);
//     }
// }

/*
 *addr: 0~3FFF
 *Note: len of bs128 must be SIZE_BYTES_PER_PAGE=128
 */
bool writeProgramMemPage(uint16_t addr, uint8_t *bs128)
{
    /*load mem page*/
    for (uint8_t i = 0, j = 0; i < SIZE_WORDS_PER_PAGE; i++, j += 2)
    {
        /*load page low byte*/
        sendCmd(0x40, 0x00, i, bs128[j]);
        /*load page high byte*/
        sendCmd(0x48, 0x00, i, bs128[j + 1]);
    }
    /*write one page*/
    sendCmd(0x4C, addr >> 8, addr, 0);
    delay(5 * 2);
    return waitForBusy();
}

/*
-------------------EEPROM 1kbytes--------------------
256 pages 4bytes/page
PCPAGE EEA[9:2]
PCWORD EEA[1:0]
*/
/*addr: 0 ~ 0b0000 00xx xxxx xxxx*/
uint8_t readEEPROMMemory(uint16_t addr)
{
    SPI_SEND8(0xA0);
    SPI_SEND8(addr >> 8);
    SPI_SEND8(addr);
    SPI_SEND8(0x00);
    return _GET_SPI_REG();
}

/*
*addr: 0b0000 00xx xxxx xx00
*Note: len of bs4 must be SIZE_BYTES_PER_EEPROM_PAGE=4
*/
bool writeEEPROMMemPage(uint16_t addr, uint8_t *bs4)
{
    /*load EEPROM Mem Page*/
    for (uint8_t i = 0; i < SIZE_BYTES_PER_EEPROM_PAGE; i++)
    {
        sendCmd(0xC1, 0x00, i, bs4[i]);
    }
    /*write EEPROM Mem Page*/
    sendCmd(0xC2, addr >> 8, addr, 0);
    delay(4 * 2);
    return waitForBusy();
}

/*addr: 0b0000 00xx xxxx xxxx*/
bool writeEEPROMMemByte(uint16_t addr, uint8_t data)
{
    sendCmd(0xC0, addr >> 8, addr, data);
    delay(5 * 2);
    return waitForBusy();
}

bool eraseChip()
{
    sendCmd(0xAC, 0x80, 0, 0);
    delay(9 * 2);
    return waitForBusy();
}

bool ATmega328EnterDebugMode()
{
    // SPI 初始化 时钟频率
    // 注意spi时钟频率不要设置太高，空白的芯片默认仅设置了1M的时钟频率
    initSPIMaster(MySPI_Fosc_1_128);

    // 3号引脚输出 8M PWM 用于给目标芯片提供时钟
    enableXTAL1CLK();

    // Power-up
    MYPIN_WRITE_MODE(PIN_RST);

    MYPIN_WRITE_HIGH(PIN_RST);
    delay(1);
    MYPIN_WRITE_LOW(PIN_RST);

    return enableProgram();
}

void ATmega328ExitDebugMode()
{
    MYPIN_WRITE_LOW(PIN_RST);
    MYPIN_READ_MODE(PIN_RST);
    disalbeXTAL1CLK();
}