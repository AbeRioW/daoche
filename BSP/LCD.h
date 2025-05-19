#ifndef __LCD_H__
#define __LCD_H__

#include "gpio.h"
#include "spi.h"

#define LCD_WIDTH       240
#define LCD_HEIGHT      320


// 8x16×ÖÌå±í
extern const unsigned char asc2_1206[95][12];
extern const unsigned char asc2_1608[95][16];


void LCD_Init(void);
void LCD_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void LCD_Fill(uint16_t color);
void ILI9341_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *img);
void createCheckerboard();
void LCD_ShowString(uint16_t x,uint16_t y,uint8_t size,uint8_t *p,uint8_t mode);
#endif

