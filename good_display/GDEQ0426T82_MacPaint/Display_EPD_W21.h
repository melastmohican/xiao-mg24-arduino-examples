#ifndef _DISPLAY_EPD_W21_H_
#define _DISPLAY_EPD_W21_H_

#include "Display_EPD_W21_spi.h"

#define EPD_WIDTH   800
#define EPD_HEIGHT  480
#define EPD_ARRAY   (EPD_WIDTH * EPD_HEIGHT / 8)

void lcd_chkstatus(void);
void EPD_HW_Init(void);
void EPD_WhiteScreen_White(void);
void EPD_WhiteScreen_ALL(const unsigned char *datas);
void EPD_SetRamArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void EPD_Part_Update_From_Buffer(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const unsigned char *full_buffer);
void EPD_Full_Update_From_Buffer(const unsigned char *full_buffer);
void EPD_DeepSleep(void);

#endif
