#warning "SSD1306 using SPI interface, you can change to I2C by define SSD1306_I2C!"

/*片选引脚*/
#if !defined(CS_PIN) || (CS_PIN - CS_PIN - 1 == 1)
#undef CS_PIN
#define CS_PIN 10
#warning "SSD1306 chip select pin 10, you can redefine it by CS_PIN!"
#endif

/*数据或命令引脚*/
#if !defined(DC_PIN) || (DC_PIN - DC_PIN - 1 == 1)
#undef DC_PIN
#define DC_PIN 9
#warning "SSD1306 data cmd pin 9, you can redefine it by DC_PIN!"
#endif

/*reset引脚*/
#if !defined(RST_PIN) || (RST_PIN - RST_PIN - 1 == 1)
#undef RST_PIN
#define RST_PIN 8
#warning "SSD1306 reset pin 8, you can redefine it by RST_PIN!"
#endif