#ifndef _DISPLAY_EPD_W21_H_
#define _DISPLAY_EPD_W21_H_


#define EPD_WIDTH   152 
#define EPD_HEIGHT  296
#define EPD_ARRAY  5624


//Full screen update display
void EPD_HW_Init(void); 
void EPD_HW_Init_180(void);	
void EPD_WhiteScreen_ALL(const unsigned char* datasBW,const unsigned char* datasRW);
void EPD_WhiteScreen_White(void);
void EPD_DeepSleep(void);
//Partial update display 
void EPD_SetRAMValue_BaseMap(const unsigned char* datasBW,const unsigned char* datasRW);
void EPD_Dis_Part(unsigned int x_start,unsigned int y_start,const unsigned char * datas,unsigned int PART_COLUMN,unsigned int PART_LINE);
//Num display
void EPD_Dis_Part_Num(unsigned int x_startA,unsigned int y_startA,const unsigned char * datasA,
	                       unsigned int x_startB,unsigned int y_startB,const unsigned char * datasB,
												 unsigned int x_startC,unsigned int y_startC,const unsigned char * datasC,
												 unsigned int x_startD,unsigned int y_startD,const unsigned char * datasD,
												 unsigned int PART_COLUMN,unsigned int PART_LINE
	                      );											 

//Fast full screen update display
void EPD_HW_Init_Fast(void);
void EPD_WhiteScreen_ALL_Fast(const unsigned char* datasBW,const unsigned char* datasRW);
void EPD_WhiteScreen_White_Fast(void);
										 
#endif



