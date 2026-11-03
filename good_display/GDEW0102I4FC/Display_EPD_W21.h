#ifndef _DISPLAY_EPD_W21_H_
#define _DISPLAY_EPD_W21_H_

#define EPD_WIDTH   80
#define EPD_HEIGHT  128
#define EPD_ARRAY   1280

// Full screen refresh display functions
void EPD_Init(void); 
void EPD_Init_180(void);  
void EPD_WhiteScreen_ALL(const unsigned char *datas);
void EPD_WhiteScreen_White(void);
void EPD_WhiteScreen_Black(void);
void EPD_DeepSleep(void);

#endif

