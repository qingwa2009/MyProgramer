#warning "SSD1306 using I2C interface, you can change to SPI by define SSD1306_SPI!"

/*reset引脚*/
#if !defined(RST_PIN) || (RST_PIN - RST_PIN - 1 == 1)
#undef RST_PIN
#define RST_PIN 4
#warning "SSD1306 reset pin 4, you can redefine it by RST_PIN!"
#endif

/*i2c地址*/
#if !defined(SSD1306_I2C_ADDRESS) || (SSD1306_I2C_ADDRESS - SSD1306_I2C_ADDRESS - 1 == 1)
#undef SSD1306_I2C_ADDRESS
#include "SSD1306.h"
#define SSD1306_I2C_ADDRESS SSD1306_I2C_ADDR
#warning "SSD1306 I2C address can be defined by SSD1306_I2C_ADDRESS!"
#endif