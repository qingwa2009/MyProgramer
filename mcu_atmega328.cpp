/*还没写完*/
#include <Arduino.h>
#include "myProgramer.h"
#include "./mylib/myUtil.h"
#include "atmega328/myATmega328.h"

void handleHelp()
{
    Serial.println(F("/d      enter debug mode"));
    Serial.println(F("/r      exit debug mode"));
    Serial.println(F("/gfh     get fuse high bits"));
    Serial.println(F("/gfl     get fuse low bits"));
    Serial.println(F("/gfe     get fuse ext bits"));
    Serial.println(F("/gl      get lock bits"));
    Serial.println(F("/gc      get calibration byte"));
    Serial.println(F("/gi      get signature byte"));
    Serial.println(F("/ga addr     read flash rom(addr: 0000~3FF0)"));
    Serial.println(F("/ge addr     read eeprom(addr: 0000~03F0)"));

    Serial.println(F("/sfh xx     set fuse high bits(x: 0~F)"));
    Serial.println(F("/sfl xx     set fuse low bits (x: 0~F)"));
    Serial.println(F("/sfe xx     set fuse ext bits (x: 0~F)"));
    Serial.println(F("/sl xx      set lock bits !!! (x: 0~F)"));

    Serial.println(F("/se addr xx  write eeprom (addr: 0000~3FFF x: 0~F)"));
}

void printlnHex16(const __FlashStringHelper *preStr, uint16_t ret16)
{
    char hex[3] = {0, 0, 0};
    Byte2Hex(ret16 >> 8, &hex[0], &hex[1]);
    Serial.print(preStr);
    Serial.print(hex);
    Byte2Hex(ret16 & 0x00FF, &hex[0], &hex[1]);
    Serial.println(hex);
}

void printHex16(const __FlashStringHelper *preStr, uint16_t ret16)
{
    char hex[3] = {0, 0, 0};
    Byte2Hex(ret16 >> 8, &hex[0], &hex[1]);
    Serial.print(preStr);
    Serial.print(hex);
    Byte2Hex(ret16 & 0x00FF, &hex[0], &hex[1]);
    Serial.print(hex);
}

void printHex8(const __FlashStringHelper *preStr, uint8_t v)
{
    char hex[3] = {0, 0, 0};
    Byte2Hex(v, &hex[0], &hex[1]);
    Serial.print(preStr);
    Serial.print(hex);
}

void printlnHex8(const __FlashStringHelper *preStr, uint8_t v)
{
    char hex[3] = {0, 0, 0};
    Byte2Hex(v, &hex[0], &hex[1]);
    Serial.print(preStr);
    Serial.println(hex);
}

void handleReadLock(char *buf, uint8_t len)
{
    Serial.println(F("lockBits: "));
    uint8_t b = readLockBits();
    Serial.print(F("5(BLB12)Boot lock bit: \t"));
    Serial.println((b & 0b00100000) ? F("1(unprogrammed)") : F("0(programmed)"));
    Serial.print(F("4(BLB11)Boot lock bit: \t"));
    Serial.println((b & 0b00010000) ? F("1(unprogrammed)") : F("0(programmed)"));
    Serial.print(F("3(BLB02)Boot lock bit: \t"));
    Serial.println((b & 0b00001000) ? F("1(unprogrammed)") : F("0(programmed)"));
    Serial.print(F("2(BLB01)Boot lock bit: \t"));
    Serial.println((b & 0b00000100) ? F("1(unprogrammed)") : F("0(programmed)"));
    Serial.print(F("1(LB2)lock bit: \t\t"));
    Serial.println((b & 0b00000010) ? F("1(unprogrammed)") : F("0(programmed)"));
    Serial.print(F("0(LB1)lock bit: \t\t"));
    Serial.println((b & 0b00000001) ? F("1(unprogrammed)") : F("0(programmed)"));
}

void handleReadCalibration(char *buf, uint8_t len)
{
    Serial.print(F("Calibration byte: "));
    printlnHex8(F("0x"), readCalibrationByte());
}

void handleReadSignature(char *buf, uint8_t len)
{
    Serial.println(F("Signature: "));
    Serial.print(F("0x000\t"));
    Serial.print(F("0x001\t"));
    Serial.println(F("0x002"));
    printHex8(F(" 0x"), readSignatureByte(0x00));
    Serial.print(F("\t"));
    printHex8(F(" 0x"), readSignatureByte(0x01));
    Serial.print(F("\t"));
    printlnHex8(F(" 0x"), readSignatureByte(0x02));
}
void printFuseLowBits(uint8_t b)
{
    Serial.print(F("7(CKDIV8)Divide clock by 8: \t"));
    Serial.println((b & 0b10000000) ? F("1(unprogrammed)") : F("0(programmed)"));
    Serial.print(F("6(CKOUT)Clock output: \t\t"));
    Serial.println((b & 0b01000000) ? F("1(unprogrammed)") : F("0(programmed)"));
    Serial.print(F("5(SUT1)Select start-up time: \t"));
    Serial.println((b & 0b00100000) ? F("1(unprogrammed)") : F("0(programmed)"));
    Serial.print(F("4(SUT0)Select start-up time: \t"));
    Serial.println((b & 0b00010000) ? F("1(unprogrammed)") : F("0(programmed)"));
    Serial.print(F("3(CKSEL3)Select clock source: \t"));
    Serial.println((b & 0b00001000) ? F("1(unprogrammed)") : F("0(programmed)"));
    Serial.print(F("2(CKSEL2)Select clock source: \t"));
    Serial.println((b & 0b00000100) ? F("1(unprogrammed)") : F("0(programmed)"));
    Serial.print(F("1(CKSEL1)Select clock source: \t"));
    Serial.println((b & 0b00000010) ? F("1(unprogrammed)") : F("0(programmed)"));
    Serial.print(F("0(CKSEL0)Select clock source: \t"));
    Serial.println((b & 0b00000001) ? F("1(unprogrammed)") : F("0(programmed)"));
}
void printFuseHighBits(uint8_t b)
{
    Serial.print(F("7(RSTDISBL)External reset disable: \t\t"));
    Serial.println((b & 0b10000000) ? F("1(unprogrammed)") : F("0(programmed)"));
    Serial.print(F("6(DWEN)debugWIRE enable: \t\t\t"));
    Serial.println((b & 0b01000000) ? F("1(unprogrammed)") : F("0(programmed)"));
    Serial.print(F("5(SPIEN)Enable serial program: \t\t\t"));
    Serial.println((b & 0b00100000) ? F("1(unprogrammed)") : F("0(programmed)"));
    Serial.print(F("4(WDTON)Watchdog timer always On: \t\t"));
    Serial.println((b & 0b00010000) ? F("1(unprogrammed)") : F("0(programmed)"));
    Serial.print(F("3(EESAVE)EEPROM is preserved when chip erase: \t"));
    Serial.println((b & 0b00001000) ? F("1(unprogrammed)") : F("0(programmed)"));
    Serial.print(F("2(BOOTSZ1)Select boot size: \t\t\t"));
    Serial.println((b & 0b00000100) ? F("1(unprogrammed)") : F("0(programmed)"));
    Serial.print(F("1(BOOTSZ0)Select boot size: \t\t\t"));
    Serial.println((b & 0b00000010) ? F("1(unprogrammed)") : F("0(programmed)"));
    Serial.print(F("0(BOOTRST)Select reset vector: \t\t\t"));
    Serial.println((b & 0b00000001) ? F("1(unprogrammed)") : F("0(programmed)"));
}
void printFuseExtBits(uint8_t b)
{
    Serial.print(F("2(BODLEVEL2)Brown-out detector trigger level: \t"));
    Serial.println((b & 0b00000100) ? F("1(unprogrammed)") : F("0(programmed)"));
    Serial.print(F("1(BODLEVEL1)Brown-out detector trigger level: \t"));
    Serial.println((b & 0b00000010) ? F("1(unprogrammed)") : F("0(programmed)"));
    Serial.print(F("0(BODLEVEL0)Brown-out detector trigger level: \t"));
    Serial.println((b & 0b00000001) ? F("1(unprogrammed)") : F("0(programmed)"));
}
void handleReadFuse(char *buf, uint8_t len)
{
    switch (buf[3])
    {
    case 'l':
        Serial.println(F("Fuse low: (Note: change fuse will not effect until leaves programming mode)"));
        printFuseLowBits(readFuseLowBits());
        break;
    case 'h':
        Serial.println(F("Fuse high: (Note: change fuse will not effect until leaves programming mode)"));
        printFuseHighBits(readFuseHighBits());
        break;
    case 'e':
        Serial.println(F("Fuse ext: (Note: change fuse will not effect until leaves programming mode)"));
        printFuseExtBits(readFuseExtBits());
        break;
    default:
        Serial.println(F("ERR /gf[h|l|e]"));
        break;
    }
}
void handleReadFlash(char *buf, uint8_t len)
{
    uint8_t addrH, addrL;
    uint16_t addr;

    if (!Hex2Byte(buf[4], buf[5], &addrH))
        goto HANDLE_ERR;
    if (!Hex2Byte(buf[6], buf[7], &addrL))
        goto HANDLE_ERR;
    addr = (addrH << 8) | addrL;

    printHex16(F("FLASH "), addr);
    Serial.print(F(":"));

    for (uint8_t i = 0; i < 16; i++)
    {
        printHex16(F(" "), (readProgramMemoryHighByte(addr) << 8) | readProgramMemoryLowByte(addr));
        addr++;
    }
    Serial.println("");

    return;
HANDLE_ERR:
    Serial.println(F("ERR /ga xxxx (0000~3FF0)"));
}
void handleReadEEPROM(char *buf, uint8_t len)
{
    uint8_t addrH, addrL;
    uint16_t addr;

    if (!Hex2Byte(buf[4], buf[5], &addrH))
        goto HANDLE_ERR;
    if (!Hex2Byte(buf[6], buf[7], &addrL))
        goto HANDLE_ERR;
    addr = (addrH << 8) | addrL;

    printHex16(F("EEPROM "), addr);
    Serial.print(F(":"));
    for (uint8_t i = 0; i < 16; i++)
    {
        printHex8(F(" "), readEEPROMMemory(addr));
        addr++;
    }
    Serial.println("");
    return;
HANDLE_ERR:
    Serial.println(F("ERR /ge xxxx (0000~03F0)"));
}
void handleWriteEEPROM(char *buf, uint8_t len)
{
    uint8_t addrH, addrL;
    uint16_t addr;
    uint8_t value;

    if (!Hex2Byte(buf[4], buf[5], &addrH))
        goto HANDLE_ERR;
    if (!Hex2Byte(buf[6], buf[7], &addrL))
        goto HANDLE_ERR;
    addr = (addrH << 8) | addrL;

    if (!Hex2Byte(buf[9], buf[10], &value))
        goto HANDLE_ERR;

    printHex16(F("Write EEPROM "), addr);
    printlnHex8(F(": 0x"), value);
    if (!writeEEPROMMemByte(addr, value))
    {
        Serial.println(F("Write EEPROM timeout!"));
    }
    else
    {
        Serial.println(F("OK!"));
    }
    return;
HANDLE_ERR:
    Serial.println(F("ERR /se addr xx"));
}
void handleRead(char *buf, uint8_t len)
{
    switch (buf[2])
    {
    case 'f':
        handleReadFuse(buf, len);
        break;
    case 'l':
        handleReadLock(buf, len);
        break;
    case 'c':
        handleReadCalibration(buf, len);
        break;
    case 'i':
        handleReadSignature(buf, len);
        break;
    case 'a':
        handleReadFlash(buf, len);
        break;
    case 'e':
        handleReadEEPROM(buf, len);
        break;
    default:
        Serial.println(F("ERR /g[f|l|c|i|a|e]"));
        break;
    }
}
void handleWriteFuse(char *buf, uint8_t len)
{
    uint8_t b;
    bool success;
    if (!Hex2Byte(buf[5], buf[6], &b))
        goto HANDLE_ERR;
    switch (buf[3])
    {
    case 'l':
        Serial.print(F("set fuse low: "));
        printlnHex8(F("0x"), b);
        success = writeFuseLowBits(b);
        break;
    case 'h':
        Serial.print(F("set fuse high: "));
        printlnHex8(F("0x"), b);
        success = writeFuseHighBits(b);
        break;
    case 'e':
        Serial.print(F("set fuse ext: "));
        printlnHex8(F("0x"), b);
        success = writeFuseExtBits(b);
        break;
    default:
        goto HANDLE_ERR;
        break;
    }
    if (!success)
    {
        Serial.println(F("Set fuse timeout!"));
    }
    else
    {
        Serial.println(F("OK!"));
    }

    return;
HANDLE_ERR:
    Serial.println(F("ERR /sf[l|h|e] xx"));
}

/*I dont test this method*/
void handleWriteLock(char *buf, uint8_t len)
{
    uint8_t b;
    if (!Hex2Byte(buf[4], buf[5], &b))
        goto HANDLE_ERR;

    Serial.print(F("set lock bits: "));
    printlnHex8(F("0x"), b);
    if (!writeLockBits(b))
    {
        Serial.println(F("Set lock bits timeout!"));
    }
    else
    {
        Serial.println(F("OK!"));
    }
    return;
HANDLE_ERR:
    Serial.println(F("ERR /sl xx"));
}

void handleWrite(char *buf, uint8_t len)
{
    switch (buf[2])
    {
    case 'f':
        handleWriteFuse(buf, len);
        break;
    case 'l':
        handleWriteLock(buf, len);
        break;
    case 'e':
        handleWriteEEPROM(buf, len);
        break;
    default:
        Serial.println(F("ERR /s[f|l|e]"));
        break;
    }
}

/*退出debug模式，进入正常模式*/
void exitDebugMode(char *buf, uint8_t len)
{
    ATmega328ExitDebugMode();
    Serial.println(F("exit debug mode!"));
}

/*进入debug模式*/
void enterDebugMode(char *buf, uint8_t len)
{
    if (ATmega328EnterDebugMode())
    {
        Serial.println(F("enter debug mode!"));
    }
    else
    {
        Serial.println(F("enter debug mode failed!"));
    }
}

void myprogramer_setup()
{
    handlers[HINDEX('r')] = exitDebugMode;
    handlers[HINDEX('d')] = enterDebugMode;
    handlers[HINDEX('g')] = handleRead;
    handlers[HINDEX('s')] = handleWrite;

    Serial.println(F("you should input /d to enter debug mode first!"));
}

uint8_t buf128[SIZE_BYTES_PER_PAGE];
/*0~3F*/
uint8_t buf128Ind;
/*0~7FFE the LSB must be zero*/
uint16_t offsetBytes;
bool beforeProgram(bool mustEraseChip)
{
    Serial.println(F("before program!"));
    buf128Ind = 0;
    offsetBytes = 0;

    if (mustEraseChip)
    {
        Serial.println(F("chip erasing!"));
        if (!eraseChip())
        {
            Serial.println(F("Warning, chip erase timeout!"));
        }
    }
    return 1;
}

bool checkIsWriteCorrect(uint16_t addrRead, uint8_t *buf128)
{
    for (uint8_t i = 0, j = 0; i < SIZE_WORDS_PER_PAGE; i++, j += 2, addrRead++)
    {
        uint8_t lb = readProgramMemoryLowByte(addrRead);
        uint8_t hb = readProgramMemoryHighByte(addrRead);
        if (buf128[j] != lb || buf128[j + 1] != hb)
        {
            printHex16(F("addr "), addrRead);
            Serial.print(F(" : "));
            printHex8(F(""), buf128[j + 1]);
            printHex8(F(""), buf128[j]);
            Serial.print(F(" => "));
            printHex8(F(""), hb);
            printlnHex8(F(""), lb);
            return 0;
        }
    }
    return 1;
}

uint8_t programing(uint32_t addr, uint8_t *buf512, uint16_t len, bool isEOF)
{

    uint16_t pTarget = (uint16_t)addr;
    uint16_t p0 = offsetBytes + buf128Ind;

    if (pTarget < p0)
        return ERR_PROGRAM_ADDR_INVALID;

    uint8_t pMax = offsetBytes + SIZE_BYTES_PER_PAGE;
    if (buf128Ind > 0 && pMax <= pTarget) /*烧写上次没烧的页，并且新的烧录地址不在同一页*/
    {
        memset(&buf128[buf128Ind], 0xFF, SIZE_BYTES_PER_PAGE - buf128Ind); /*填充0xFF到页剩余的部分*/
        uint16_t addrProgram = offsetBytes >> 1;
        if (!writeProgramMemPage(addrProgram, buf128))
            return ERR_PROGRAM_WRITE_TIMEOUT;
        if (!checkIsWriteCorrect(addrProgram, buf128))
            return ERR_PROGRAM_READ_BACK_UNMATCH;

        buf128Ind = 0;
        offsetBytes += SIZE_BYTES_PER_PAGE;
    }

    /*编程页地址移动到pTarget附近*/
    while (offsetBytes + SIZE_BYTES_PER_PAGE <= pTarget)
    {
        offsetBytes += SIZE_BYTES_PER_PAGE;
    }

    /*按页填充0xFF直到到新的烧录地址*/
    p0 = offsetBytes + buf128Ind;
    uint8_t space = pTarget - p0;
    if (space > 0)
    {
        memset(buf128 + buf128Ind, 0xFF, space);
        buf128Ind += space;
    }

    uint16_t buf512Ind = 0;
    space = (SIZE_BYTES_PER_PAGE - buf128Ind);
    while (len >= space) /*一页烧不完*/
    {
        memcpy(&buf128[buf128Ind], &buf512[buf512Ind], space);
        uint16_t addrProgram = offsetBytes >> 1;
        if (!writeProgramMemPage(addrProgram, buf128))
            return ERR_PROGRAM_WRITE_TIMEOUT;
        if (!checkIsWriteCorrect(addrProgram, buf128))
            return ERR_PROGRAM_READ_BACK_UNMATCH;

        len -= space;
        buf512Ind += space;

        buf128Ind = 0;
        space = SIZE_BYTES_PER_PAGE;
        offsetBytes += SIZE_BYTES_PER_PAGE;
    }

    if (len > 0)
    {
        /*凑不齐1页，留到下一次*/
        memcpy(&buf128[buf128Ind], &buf512[buf512Ind], len);
        buf128Ind += len;
    }

    /*最后剩下没烧的部分*/
    if (isEOF && (buf128Ind > 0))
    {
        memset(&buf128[buf128Ind], 0xFF, SIZE_BYTES_PER_PAGE - buf128Ind); /*填充0xFF到页剩余的部分*/
        uint16_t addrProgram = offsetBytes >> 1;
        if (!writeProgramMemPage(addrProgram, buf128))
            return ERR_PROGRAM_WRITE_TIMEOUT;
        if (!checkIsWriteCorrect(addrProgram, buf128))
            return ERR_PROGRAM_READ_BACK_UNMATCH;

        buf128Ind = 0;
        offsetBytes += SIZE_BYTES_PER_PAGE;
    }

    return ERR_OK;
}

bool checkEEPROMIsWriteCorrect(uint16_t addrRead, uint8_t *buf4)
{
    for (uint8_t i = 0; i < SIZE_BYTES_PER_EEPROM_PAGE; i++, addrRead++)
    {
        uint8_t lb = readEEPROMMemory(addrRead);
        if (buf4[i] != lb)
        {
            printHex16(F("addr "), addrRead);
            Serial.print(F(" : "));
            printHex8(F(""), buf4[i]);
            Serial.print(F(" => "));
            printlnHex8(F(""), lb);
            return 0;
        }
    }
    return 1;
}

uint8_t programingEEPROM(uint32_t addr, uint8_t *buf512, uint16_t len, bool isEOF)
{
    uint16_t pTarget = (uint16_t)addr;
    uint16_t p0 = offsetBytes + buf128Ind;

    if (pTarget < p0)
        return ERR_PROGRAM_ADDR_INVALID;

    uint8_t pMax = offsetBytes + SIZE_BYTES_PER_EEPROM_PAGE;
    if (buf128Ind > 0 && pMax <= pTarget) /*烧写上次没烧的页，并且新的烧录地址不在同一页*/
    {
        memset(&buf128[buf128Ind], 0xFF, SIZE_BYTES_PER_EEPROM_PAGE - buf128Ind); /*填充0xFF到页剩余的部分*/
        uint16_t addrProgram = offsetBytes;
        if (!writeEEPROMMemPage(addrProgram, buf128))
            return ERR_PROGRAM_WRITE_TIMEOUT;
        if (!checkEEPROMIsWriteCorrect(addrProgram, buf128))
            return ERR_PROGRAM_READ_BACK_UNMATCH;

        buf128Ind = 0;
        offsetBytes += SIZE_BYTES_PER_EEPROM_PAGE;
    }

    /*编程页地址移动到pTarget附近*/
    while (offsetBytes + SIZE_BYTES_PER_EEPROM_PAGE <= pTarget)
    {
        offsetBytes += SIZE_BYTES_PER_EEPROM_PAGE;
    }

    /*按页填充0xFF直到到新的烧录地址*/
    p0 = offsetBytes + buf128Ind;
    uint8_t space = pTarget - p0;
    if (space > 0)
    {
        memset(buf128 + buf128Ind, 0xFF, space);
        buf128Ind += space;
    }

    uint16_t buf512Ind = 0;
    space = (SIZE_BYTES_PER_EEPROM_PAGE - buf128Ind);
    while (len >= space) /*一页烧不完*/
    {
        memcpy(&buf128[buf128Ind], &buf512[buf512Ind], space);
        uint16_t addrProgram = offsetBytes;
        if (!writeEEPROMMemPage(addrProgram, buf128))
            return ERR_PROGRAM_WRITE_TIMEOUT;
        if (!checkEEPROMIsWriteCorrect(addrProgram, buf128))
            return ERR_PROGRAM_READ_BACK_UNMATCH;

        len -= space;
        buf512Ind += space;

        buf128Ind = 0;
        space = SIZE_BYTES_PER_EEPROM_PAGE;
        offsetBytes += SIZE_BYTES_PER_EEPROM_PAGE;
    }

    if (len > 0)
    {
        /*凑不齐1页，留到下一次*/
        memcpy(&buf128[buf128Ind], &buf512[buf512Ind], len);
        buf128Ind += len;
    }

    /*最后剩下没烧的部分*/
    if (isEOF && (buf128Ind > 0))
    {
        memset(&buf128[buf128Ind], 0xFF, SIZE_BYTES_PER_EEPROM_PAGE - buf128Ind); /*填充0xFF到页剩余的部分*/
        uint16_t addrProgram = offsetBytes;
        if (!writeEEPROMMemPage(addrProgram, buf128))
            return ERR_PROGRAM_WRITE_TIMEOUT;
        if (!checkEEPROMIsWriteCorrect(addrProgram, buf128))
            return ERR_PROGRAM_READ_BACK_UNMATCH;

        buf128Ind = 0;
        offsetBytes += SIZE_BYTES_PER_EEPROM_PAGE;
    }

    return ERR_OK;
}

uint8_t programingBin(uint32_t addr, uint8_t *buf)
{

    for (uint16_t i = 0; i < 512; i += SIZE_BYTES_PER_PAGE)
    {
        bool isAllFF = 1;
        uint16_t jm = i + SIZE_BYTES_PER_PAGE;
        for (uint16_t j = i; j < jm; j++)
        {
            if (buf[j] != 0xFF)
            {
                isAllFF = 0;
                break;
            }
        }
        if (!isAllFF)
        {
            uint16_t addrProgram = addr >> 1;
            if (!writeProgramMemPage(addrProgram, &buf[i]))
                return ERR_PROGRAM_WRITE_TIMEOUT;
            if (!checkIsWriteCorrect(addrProgram, &buf[i]))
                return ERR_PROGRAM_READ_BACK_UNMATCH;
        }
        addr += SIZE_BYTES_PER_PAGE;
    }
    return ERR_OK;
}
uint8_t programingBinEEPROM(uint32_t addr, uint8_t *buf)
{
    for (uint16_t i = 0; i < 512; i += SIZE_BYTES_PER_EEPROM_PAGE)
    {
        uint16_t addrProgram = addr;
        if (!writeEEPROMMemPage(addrProgram, &buf[i]))
            return ERR_PROGRAM_WRITE_TIMEOUT;
        if (!checkEEPROMIsWriteCorrect(addrProgram, &buf[i]))
            return ERR_PROGRAM_READ_BACK_UNMATCH;

        addr += SIZE_BYTES_PER_EEPROM_PAGE;
    }
    return ERR_OK;
}
void afterProgram()
{
    Serial.println(F("after program!"));
    exitDebugMode(0, 0);
}