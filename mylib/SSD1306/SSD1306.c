#include <Arduino.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include "../MyPin.h"
#include "SSD1306.h"
#include "glcdfont.h"

#if defined(SSD1306_SPI)
#include "SSD1306_SPI.h"
#else
#include "SSD1306_I2C.h"
#endif

/*必须定义reset引脚*/
#ifndef RST_PIN
#define RST_PIN RST_PIN
#endif

int16_t SCREEN_W = SSD1306_LCD_WIDTH;
int16_t SCREEN_H = SSD1306_LCD_HEIGH;
int16_t SCREEN_MAX_X = SSD1306_LCD_MAX_X;
int16_t SCREEN_MAX_Y = SSD1306_LCD_MAX_Y;

/*01111100 lbit=2, rbit=6 */
#define TRIM_BYTE(lbit, rbit) (_TRIM_MASK[(lbit)] & _TRIM_MASK[9 + (rbit)])
#define TRIM_LBYTE(lbit) (_TRIM_MASK[(lbit)])
#define TRIM_RBYTE(rbit) (_TRIM_MASK[9 + (rbit)])

static uint8_t buffer[SSD1306_BUFFER_SIZE];
static uint8_t screenDir;

uint8_t *getDrawBuffer()
{
    return buffer;
}
static uint8_t _TRIM_MASK[] = {
    0b11111111,
    0b11111110,
    0b11111100,
    0b11111000,
    0b11110000,
    0b11100000,
    0b11000000,
    0b10000000,
    0b00000000,
    0b00000001,
    0b00000011,
    0b00000111,
    0b00001111,
    0b00011111,
    0b00111111,
    0b01111111,
    0b11111111,
};

void inverseDisplay(uint8_t inverse)
{
    _writeCmd(inverse ? 0xA7 : 0xA6);
}

void whiteScreen(uint8_t on)
{
    _writeCmd(on ? 0xA5 : 0xA4);
}

void displayON(uint8_t on)
{
    _writeCmd(on ? CMD_DISPLAY_ON : CMD_DISPLAY_OFF);
    delay(100);
}

void setDisplayStartLine(uint8_t r)
{
    _writeCmd(0x40 | (0x3F & r));
}

void setContrast(uint8_t v)
{
    _writeCmd(CMD_SET_CONTRAST);
    _writeCmd(v);
}

void setScreenDir(uint8_t dir)
{
    dir %= 4;
    screenDir = dir;
    switch (dir)
    {
    case 0:
        _writeCmd(0xA0);
        _writeCmd(0xC0);
        SCREEN_W = SSD1306_LCD_WIDTH;
        SCREEN_H = SSD1306_LCD_HEIGH;
        SCREEN_MAX_X = SSD1306_LCD_MAX_X;
        SCREEN_MAX_Y = SSD1306_LCD_MAX_Y;
        break;
    case 1:
        _writeCmd(0xA1);
        _writeCmd(0xC0);
        SCREEN_W = SSD1306_LCD_HEIGH;
        SCREEN_H = SSD1306_LCD_WIDTH;
        SCREEN_MAX_X = SSD1306_LCD_MAX_Y;
        SCREEN_MAX_Y = SSD1306_LCD_MAX_X;
        break;
    case 2:
        _writeCmd(0xA1);
        _writeCmd(0xC8);
        SCREEN_W = SSD1306_LCD_WIDTH;
        SCREEN_H = SSD1306_LCD_HEIGH;
        SCREEN_MAX_X = SSD1306_LCD_MAX_X;
        SCREEN_MAX_Y = SSD1306_LCD_MAX_Y;
        break;
    default:
        _writeCmd(0xA0);
        _writeCmd(0xC8);
        SCREEN_W = SSD1306_LCD_HEIGH;
        SCREEN_H = SSD1306_LCD_WIDTH;
        SCREEN_MAX_X = SSD1306_LCD_MAX_Y;
        SCREEN_MAX_Y = SSD1306_LCD_MAX_X;
        break;
    }
}
void startScrollLeft()
{
}

void startScrollRight()
{
}

void initSSD1306()
{
    _INIT();
    MYPIN_WRITE_MODE(RST_PIN);
    MYPIN_WRITE_LOW(RST_PIN);
    MYPIN_WRITE_HIGH(RST_PIN);

    displayON(0);
    _writeCmd(0xD5); /*Set Osc Frequency D5h, 80h*/

    _writeCmd(0x80);

    _writeCmd(0xA8); /*Set MUX Ratio 0xA8, 0x3F*/
    _writeCmd(0x3F);

    _writeCmd(0xD3); /*Set Display Offset 0xD3, 0x00*/
    _writeCmd(0x00);

    _writeCmd(0x40); /*Set Display Start Line 0x40*/

    _writeCmd(0x8D); /*Enable charge pump regulator 8Dh, 14h*/
    _writeCmd(0x14);

    _writeCmd(0x20); /*Set Memory Addressing Mode*/
    _writeCmd(0x00); /*Horizontal addressing mode*/

    _writeCmd(0xA0); /*Set Segment re-map A0h/A1h*/
    _writeCmd(0xC0); /*Set COM Output Scan Direction C0h/C8h*/

    _writeCmd(0xDA); /*Set COM Pins hardware configuration DAh, 12*/
    _writeCmd(0x12);

    setContrast(0xCF); /*Set Contrast Control 81h, 7Fh*/

    _writeCmd(0xD9); /*Set Pre-charge Period*/
    _writeCmd(0xF1);

    _writeCmd(0xDB); /*Set VCOMH Deselect Level*/
    _writeCmd(0x40);

    whiteScreen(0); /*Disable Entire Display On A4h*/

    inverseDisplay(0); /*Set Normal Display A6h*/

    setScreenDir(0);
    displayON(1);

    clearDisplay(0);
}

void setScroll()
{
    // _writeCmd(0x2E); /*Deactivate Scroll*/
    // _writeCmd(0x29); /*Continuous Vertical and Horizontal Scroll Setup*/
    // _writeCmd(0x00); /*dummy*/
    // _writeCmd(0x00); /*start page addr*/
    // _writeCmd(0x00); /*5 frame each scroll step*/
    // _writeCmd(0x07); /*end page addr*/
    // _writeCmd(0x01); /*vertical scrolling offset*/
    // _writeCmd(0x2F); /*Activate Scroll*/
}

void drawPixel(int16_t x, int16_t y, enum Color color)
{
    if (x > SCREEN_MAX_X || y > SCREEN_MAX_Y || x < 0 || y < 0)
        return;
    uint8_t v = (1 << (y & 7));
    uint16_t ind = (y / 8) * SCREEN_W + x;
    switch (color)
    {
    case WHITE:
        buffer[ind] |= v;
        break;
    case BLACK:
        buffer[ind] &= ~v;
        break;
    case INVERSE:
        buffer[ind] ^= v;
        break;
    default:
        break;
    }
}
void drawHorizontalLine(int16_t x, int16_t y, int16_t length, enum Color color)
{
    if (x > SCREEN_MAX_X || y < 0 || y > SCREEN_MAX_Y)
        return;
    length += x;
    if (length <= 0)
        return;

    x = x > 0 ? x : 0;

    length = length < SCREEN_W ? length : SCREEN_W;
    uint8_t v = (1 << (y & 7));
    uint16_t ind = (y / 8) * SCREEN_W + x;

    switch (color)
    {
    case WHITE:
        for (; x < length; x++)
        {
            buffer[ind++] |= v;
        }
        break;
    case BLACK:
        for (; x < length; x++)
        {
            buffer[ind++] &= ~v;
        }
        break;
    case INVERSE:
        for (; x < length; x++)
        {
            buffer[ind++] ^= v;
        }
        break;
    default:
        break;
    }
}
void drawVerticalLine(int16_t x, int16_t y, int16_t length, enum Color color)
{
    if (x < 0 || x > SCREEN_MAX_X || y > SCREEN_MAX_Y)
        return;
    length += y - 1;
    if (length < 0)
        return;

    y = y > 0 ? y : 0;
    length = length < SCREEN_H ? length : SCREEN_MAX_Y;

    uint8_t r0 = y / 8;
    uint8_t r1 = length / 8;
    uint8_t offset0 = y & 7;
    uint8_t offset1 = length & 7;

    uint16_t ind = r0 * SCREEN_W + x;
    if (r0 == r1)
    {
        switch (color)
        {
        case WHITE:
            buffer[ind] |= TRIM_BYTE(offset0, offset1);
            break;
        case BLACK:
            buffer[ind] &= ~TRIM_BYTE(offset0, offset1);
            break;
        case INVERSE:
            buffer[ind] ^= TRIM_BYTE(offset0, offset1);
            break;
        default:
            break;
        }
    }
    else
    {
        switch (color)
        {
        case WHITE:
            buffer[ind] |= TRIM_LBYTE(offset0);
            ind += SCREEN_W;
            for (r0++; r0 < r1; r0++)
            {
                buffer[ind] |= 0xFF;
                ind += SCREEN_W;
            }
            buffer[ind] |= TRIM_RBYTE(offset1);
            break;
        case BLACK:
            buffer[ind] &= ~TRIM_LBYTE(offset0);
            ind += SCREEN_W;
            for (r0++; r0 < r1; r0++)
            {
                buffer[ind] &= 0;
                ind += SCREEN_W;
            }
            buffer[ind] &= ~TRIM_RBYTE(offset1);
            break;
        case INVERSE:
            buffer[ind] ^= TRIM_LBYTE(offset0);
            ind += SCREEN_W;
            for (r0++; r0 < r1; r0++)
            {
                buffer[ind] ^= 0xFF;
                ind += SCREEN_W;
            }
            buffer[ind] ^= TRIM_RBYTE(offset1);
            break;
        default:
            break;
        }
    }
}

void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, enum Color color)
{
    drawHorizontalLine(x, y, w, color);
    drawHorizontalLine(x, y + h - 1, w, color);
    drawVerticalLine(x, y, h, color);
    drawVerticalLine(x + w - 1, y, h, color);
}

void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, enum Color color)
{
    if (x > SCREEN_MAX_X || y > SCREEN_MAX_Y)
        return;
    w += x - 1;
    h += y - 1;
    if (w < 0 || h < 0)
        return;

    x = x > 0 ? x : 0;
    y = y > 0 ? y : 0;

    w = w < SCREEN_W ? w : SCREEN_MAX_X;
    h = h < SCREEN_H ? h : SCREEN_MAX_Y;

    uint8_t r0 = y / 8;
    uint8_t r1 = h / 8;
    uint8_t offset0 = y & 7;
    uint8_t offset1 = h & 7;

    uint16_t ind;
    if (r0 == r1)
    {
        ind = r0 * SCREEN_W + x;
        switch (color)
        {
        case WHITE:
            for (; x <= w; x++)
                buffer[ind++] |= TRIM_BYTE(offset0, offset1);
            break;
        case BLACK:
            for (; x <= w; x++)
                buffer[ind++] &= ~TRIM_BYTE(offset0, offset1);
            break;
        case INVERSE:
            for (; x <= w; x++)
                buffer[ind++] ^= TRIM_BYTE(offset0, offset1);
            break;
        default:
            break;
        }
    }
    else
    {
        h = r0;
        switch (color)
        {
        case WHITE:
            for (; x <= w; x++)
            {
                r0 = h;
                ind = r0 * SCREEN_W + x;
                buffer[ind] |= TRIM_LBYTE(offset0);
                ind += SCREEN_W;
                for (r0++; r0 < r1; r0++)
                {
                    buffer[ind] |= 0xFF;
                    ind += SCREEN_W;
                }
                buffer[ind] |= TRIM_RBYTE(offset1);
            }
            break;
        case BLACK:
            for (; x <= w; x++)
            {
                r0 = h;
                ind = r0 * SCREEN_W + x;
                buffer[ind] &= ~TRIM_LBYTE(offset0);
                ind += SCREEN_W;
                for (r0++; r0 < r1; r0++)
                {
                    buffer[ind] &= 0;
                    ind += SCREEN_W;
                }
                buffer[ind] &= ~TRIM_RBYTE(offset1);
            }
            break;
        case INVERSE:
            for (; x <= w; x++)
            {
                r0 = h;
                ind = r0 * SCREEN_W + x;
                buffer[ind] ^= TRIM_LBYTE(offset0);
                ind += SCREEN_W;
                for (r0++; r0 < r1; r0++)
                {
                    buffer[ind] ^= 0xFF;
                    ind += SCREEN_W;
                }
                buffer[ind] ^= TRIM_RBYTE(offset1);
            }
            break;
        default:
            break;
        }
    }
}

void drawChar(int16_t x, int16_t y, uint8_t c, int8_t scale, enum Color color)
{
    if (x > SCREEN_MAX_X || y > SCREEN_MAX_Y)
        return;
    uint16_t ind = c * 5;
    for (uint8_t i = 0; i < FONT_W; i++)
    {
        uint8_t t = pgm_read_byte(&font[ind + i]);
        for (uint8_t j = 0; j < 8; j++)
        {
            if ((t >> j) & 0x01)
                fillRect(x, y + j * scale, scale, scale, color);
        }
        x += scale;
    }
}

void drawStr(int16_t x, int16_t y, uint8_t *s, int16_t count, int8_t scale, enum Color color)
{
    if (x > SCREEN_MAX_X || y > SCREEN_MAX_Y)
        return;
    int16_t dx = (FONT_W + 1) * scale;
    for (int16_t i = 0; i < count; i++)
    {
        drawChar(x, y, s[i], scale, color);
        x += dx;
    }
}

void clearDisplay(uint8_t c)
{
    memset(buffer, c, sizeof(buffer));
}

void display()
{
    //TWISaveFrequency();
    //TWISetFrequency(SSD1306_I2C_FREQUENCY);
    _BEFORE_DISPLAY;

    _writeCmd(CMD_SET_COL_ADDR);
    _writeCmd(0);
    _writeCmd(SSD1306_LCD_MAX_X);
    _writeCmd(CMD_SET_PAGE_ADDR);
    _writeCmd(0);
    _writeCmd(SSD1306_LCD_MAX_Y / 8);

    // if (TWIMasterSyncStart(SSD1306_I2C_ADDRESS, TWI_WRITE_MODE))
    if (_START_DISPLAY)
    {
        //TWIMasterSyncWriteByte(DATA);
        _DATA_MODE_DISPLAY;
        if ((screenDir & 1) == 0)
        {
            for (uint16_t i = 0; i < SSD1306_BUFFER_SIZE; i++)
            {
                // TWIMasterSyncWriteWaitAck();
                // TWIMasterSyncWriteByte(buffer[i]);
                _SEND_DISPLAY(buffer[i]);
            }
            //TWIMasterSyncWriteWaitAck();
            _WAIT_SEND_DISPLAY;
        }
        else
        {
            for (uint16_t k = 0; k < SCREEN_W; k += 8)
            {
                for (uint16_t i = k; i < SSD1306_BUFFER_SIZE; i += SCREEN_W)
                {
                    uint8_t t, a, b, c, d, e, f, g, h;
                    a = buffer[i];
                    b = buffer[i + 1];
                    c = buffer[i + 2];
                    d = buffer[i + 3];
                    e = buffer[i + 4];
                    f = buffer[i + 5];
                    g = buffer[i + 6];
                    h = buffer[i + 7];
                    uint8_t j = 8;
                    do
                    {
                        t = ((a & 0x01) << 0) |
                            ((b & 0x01) << 1) |
                            ((c & 0x01) << 2) |
                            ((d & 0x01) << 3) |
                            ((e & 0x01) << 4) |
                            ((f & 0x01) << 5) |
                            ((g & 0x01) << 6) |
                            ((h & 0x01) << 7);
                        // TWIMasterSyncWriteWaitAck();
                        // TWIMasterSyncWriteByte(t);
                        _SEND_DISPLAY(t);
                        j--;
                        if (j == 0)
                            break;
                        a >>= 1;
                        b >>= 1;
                        c >>= 1;
                        d >>= 1;
                        e >>= 1;
                        f >>= 1;
                        g >>= 1;
                        h >>= 1;
                    } while (1);
                }
            }
            //TWIMasterSyncWriteWaitAck();
            _WAIT_SEND_DISPLAY;
        }
        //TWIMasterSyncStop();
        _STOP_DISPLAY;
    }
    else
    {
    }
    _AFTER_DISPLAY;
}
// void _display(uint8_t *buffer, uint8_t screenDir)
// {
//TWISaveFrequency();
//TWISetFrequency(SSD1306_I2C_FREQUENCY);
//     _writeCmd(CMD_SET_COL_ADDR);
//     _writeCmd(0);
//     _writeCmd(SSD1306_LCD_MAX_X);
//     _writeCmd(CMD_SET_PAGE_ADDR);
//     _writeCmd(0);
//     _writeCmd(SSD1306_LCD_MAX_Y / 8);

//     if (TWIMasterSyncStart(SSD1306_I2C_ADDRESS, TWI_WRITE_MODE))
//     {
//         TWIMasterSyncWriteByte(DATA);
//         if ((screenDir & 1) == 0)
//         {
//             for (uint16_t i = 0; i < SSD1306_BUFFER_SIZE; i++)
//             {
//                 TWIMasterSyncWriteWaitAck();
//                 TWIMasterSyncWriteByte(buffer[i]);
//             }
//             TWIMasterSyncWriteWaitAck();
//         }
//         else
//         {
//             for (uint16_t k = 0; k < SCREEN_W; k += 8)
//             {
//                 for (uint16_t i = k; i < SSD1306_BUFFER_SIZE; i += SCREEN_W)
//                 {
//                     uint8_t t, a, b, c, d, e, f, g, h;
//                     a = buffer[i];
//                     b = buffer[i + 1];
//                     c = buffer[i + 2];
//                     d = buffer[i + 3];
//                     e = buffer[i + 4];
//                     f = buffer[i + 5];
//                     g = buffer[i + 6];
//                     h = buffer[i + 7];
//                     uint8_t j = 8;
//                     do
//                     {
//                         t = ((a & 0x01) << 0) |
//                             ((b & 0x01) << 1) |
//                             ((c & 0x01) << 2) |
//                             ((d & 0x01) << 3) |
//                             ((e & 0x01) << 4) |
//                             ((f & 0x01) << 5) |
//                             ((g & 0x01) << 6) |
//                             ((h & 0x01) << 7);
//                         TWIMasterSyncWriteWaitAck();
//                         TWIMasterSyncWriteByte(t);
//                         j--;
//                         if (j == 0)
//                             break;
//                         a >>= 1;
//                         b >>= 1;
//                         c >>= 1;
//                         d >>= 1;
//                         e >>= 1;
//                         f >>= 1;
//                         g >>= 1;
//                         h >>= 1;
//                     } while (1);
//                 }
//             }
//             TWIMasterSyncWriteWaitAck();
//         }

//         TWIMasterSyncStop();
//     }
//     else
//     {
//     }
// }