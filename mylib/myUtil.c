#include <Arduino.h>
#include "myUtil.h"

void toLowerCase(char *buf, uint8_t len)
{
    for (uint8_t i = 0; i < len; i++)
    {
        char b = buf[i];
        if (b >= 'A' && b <= 'Z')
            buf[i] = b + 32;
    }
}

void toUpperCase(char *buf, uint8_t len)
{
    for (uint8_t i = 0; i < len; i++)
    {
        char b = buf[i];
        if (b >= 'a' && b <= 'z')
            buf[i] = b - 32;
    }
}

/*单字节16进制字符串转10进制 b1高4位，b0低4位*/
bool Hex2Byte(char b1, char b0, uint8_t *out_b)
{
    switch (b0)
    {
    case '0' ... '9':
        b0 = b0 - '0';
        break;
    case 'A' ... 'F':
        b0 = b0 - 'A' + 10;
        break;
    case 'a' ... 'f':
        b0 = b0 - 'a' + 10;
        break;
    default:
        return 0;
    }

    switch (b1)
    {
    case '0' ... '9':
        b1 = b1 - '0';
        break;
    case 'A' ... 'F':
        b1 = b1 - 'A' + 10;
        break;
    case 'a' ... 'f':
        b1 = b1 - 'a' + 10;
        break;
    default:
        return 0;
    }

    *out_b = b1 * 16 + b0;
    return 1;
}

/*将字节转成16进制两个字符， b1高4位，b0低4位*/
void Byte2Hex(uint8_t b, char *b1, char *b0)
{
    uint8_t bb = b >> 4;
    *b1 = bb < 10 ? '0' + bb : 'A' + bb - 10;
    bb = b & 0x0F;
    *b0 = bb < 10 ? '0' + bb : 'A' + bb - 10;
}