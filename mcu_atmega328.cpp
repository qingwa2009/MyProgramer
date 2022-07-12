/*还没写完*/
#include <Arduino.h>
#include "myProgramer.h"
#include "./mylib/myUtil.h"

void handleHelp()
{
    Serial.println(F("/fh[=xx]     get/set fuse high bits"));
    Serial.println(F("/fl[=xx]     get/set fuse low bits"));
    Serial.println(F("/fe[=xx]     get/set fuse ext bits"));
    Serial.println(F("/l      get lock bits"));
    Serial.println(F("/c      get calibration byte"));
    Serial.println(F("/s      get signature byte"));
}

void writeFuseHigh(uint8_t b)
{
    //...
}
uint8_t readFuseHigh()
{
    //...
    return 0;
}
void writeFuseLow(uint8_t b)
{
    //...
}
uint8_t readFuseLow()
{
    //...
    return 0;
}
void writeFuseExt(uint8_t b)
{
    //...
}
uint8_t readFuseExt()
{
    //...
    return 0;
}
void handleFuse(char *buf, uint8_t len)
{
    uint8_t b;
    switch (buf[2])
    {
    case 'h':
        if (buf[3] == '=')
        {
            if (Hex2Byte(buf[4], buf[5], &b))
            {
                writeFuseHigh(b);
                Serial.println("OK");
            }
            else
            {
                Serial.println("ERR VALUE");
            }
        }
        else
        {
            Serial.print("fuseHigh: ");
            Serial.println(readFuseHigh(), BIN);
        }
        break;
    case 'l':
        if (buf[3] == '=')
        {
            if (Hex2Byte(buf[4], buf[5], &b))
            {
                writeFuseLow(b);
                Serial.println("OK");
            }
            else
            {
                Serial.println("ERR VALUE");
            }
        }
        else
        {
            Serial.print("fuseLow: ");
            Serial.println(readFuseLow(), BIN);
        }
        break;
    case 'e':
        if (buf[3] == '=')
        {
            if (Hex2Byte(buf[4], buf[5], &b))
            {
                writeFuseExt(b);
                Serial.println("OK");
            }
            else
            {
                Serial.println("ERR VALUE");
            }
        }
        else
        {
            Serial.print("fuseExt: ");
            Serial.println(readFuseExt(), BIN);
        }
        break;
    default:
        Serial.println("ERR /f[h|l|e]");
        break;
    }
}

uint8_t readLock()
{
    //...
    return 0;
}
void handleLock(char *buf, uint8_t len)
{
    Serial.print("lockBits(readonly): ");
    Serial.println(readLock(), BIN);
}

uint8_t readCalibratioin()
{
    //...
    return 0;
}
void handleCalibration(char *buf, uint8_t len)
{
    Serial.print("Calibration byte(readonly): ");
    Serial.println(readCalibratioin(), BIN);
}

uint8_t readSignature(uint8_t addr)
{
    //...
    return 0;
}

void handleSignature(char *buf, uint8_t len)
{
    Serial.print("Signature(readonly): ");
    Serial.println(readSignature(0x00), BIN);
    Serial.println(readSignature(0x01), BIN);
    Serial.println(readSignature(0x02), BIN);
}

void myprogramer_setup()
{

    handlers[HINDEX('f')] = handleFuse;
    handlers[HINDEX('l')] = handleLock;
    handlers[HINDEX('c')] = handleCalibration;
    handlers[HINDEX('s')] = handleSignature;
}

void beforeProgram()
{
    Serial.println("before program!");
}

uint8_t programing(uint32_t baseAddr, HexRow *hexRow)
{
    for (uint8_t i = 0; i < hexRow->dataLen; i++)
    {
        //...
    }

    return ERR_OK;
}
uint8_t programingBin(uint32_t addr, uint8_t *buf)
{
    return ERR_OK;
}
void afterProgram()
{
    Serial.println("after program!");
}