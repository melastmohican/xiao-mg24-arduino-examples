#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"

unsigned char oldData[EPD_ARRAY];
unsigned char oldDataP[256];
unsigned char oldDataA[256];
unsigned char oldDataB[256];
unsigned char oldDataC[256];
unsigned char oldDataD[256];
unsigned char oldDataE[256]; 
unsigned char partFlag = 1;

// Busy function with 40-second timeout: UC8151D BUSY pin is active LOW (0 = busy, 1 = ready/idle).
void lcd_chkstatus(void)
{
  unsigned long start = millis();
  while (isEPD_W21_BUSY == 0)
  {
    if (millis() - start > 40000) {
      Serial.println(F("GDEW0215T12 BUSY timeout!"));
      break;
    }
    delay(5);
  }
}

// UC8151D Full refresh initialization
void EPD_Init(void)
{ 
  unsigned char i;
  for (i = 0; i < 3; i++)
  {
    EPD_W21_RST_0;    // Module reset
    delay(10);
    EPD_W21_RST_1;
    delay(10);
  } 
  lcd_chkstatus();

  EPD_W21_WriteCMD(0x00);     // panel setting
  EPD_W21_WriteDATA(0x1f);    // LUT from OTP, KW-BF KWR-AF BWROTP 0f BWOTP 1f
  EPD_W21_WriteDATA(0x0D);  

  EPD_W21_WriteCMD(0x61);     // resolution setting
  EPD_W21_WriteDATA(EPD_WIDTH);       
  EPD_W21_WriteDATA(EPD_HEIGHT / 256);
  EPD_W21_WriteDATA(EPD_HEIGHT % 256); 

  EPD_W21_WriteCMD(0x04);     // power on
  lcd_chkstatus();

  EPD_W21_WriteCMD(0x50);     // VCOM AND DATA INTERVAL SETTING      
  EPD_W21_WriteDATA(0x97);    // WBmode: VBDF 17|D7 VBDW 97 VBDB 57
}

const unsigned char lut_vcom1[] = {
  0x00, 0x19, 0x01, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00,
};

const unsigned char lut_ww1[] = {
  0x00, 0x19, 0x01, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char lut_bw1[] = {
  0x80, 0x19, 0x01, 0x00, 0x00, 0x01,  
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char lut_wb1[] = {
  0x40, 0x19, 0x01, 0x00, 0x00, 0x01,  
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char lut_bb1[] = {
  0x00, 0x19, 0x01, 0x00, 0x00, 0x01,  
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

void lut1(void)
{
  unsigned int count;
  EPD_W21_WriteCMD(0x20);
  for (count = 0; count < 44; count++) {
    EPD_W21_WriteDATA(lut_vcom1[count]);
  }

  EPD_W21_WriteCMD(0x21);
  for (count = 0; count < 42; count++) {
    EPD_W21_WriteDATA(lut_ww1[count]);
  }   

  EPD_W21_WriteCMD(0x22);
  for (count = 0; count < 42; count++) {
    EPD_W21_WriteDATA(lut_bw1[count]);
  } 

  EPD_W21_WriteCMD(0x23);
  for (count = 0; count < 42; count++) {
    EPD_W21_WriteDATA(lut_wb1[count]);
  } 

  EPD_W21_WriteCMD(0x24);
  for (count = 0; count < 42; count++) {
    EPD_W21_WriteDATA(lut_bb1[count]);
  }   
}

void EPD_Init_Part(void)
{
  unsigned char i;
  for (i = 0; i < 3; i++)
  {
    EPD_W21_RST_0;    // Module reset
    delay(10);
    EPD_W21_RST_1;
    delay(10);
  } 
  lcd_chkstatus();

  EPD_W21_WriteCMD(0x01);     // POWER SETTING 
  EPD_W21_WriteDATA(0x03);           
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA(0x2b);
  EPD_W21_WriteDATA(0x2b);
  EPD_W21_WriteDATA(0x03);

  EPD_W21_WriteCMD(0x06);     // boost soft start
  EPD_W21_WriteDATA(0x17);    // A
  EPD_W21_WriteDATA(0x17);    // B
  EPD_W21_WriteDATA(0x17);    // C       

  EPD_W21_WriteCMD(0x00);     // panel setting
  EPD_W21_WriteDATA(0xbf);    // LUT from OTP
  EPD_W21_WriteDATA(0x0D);  

  EPD_W21_WriteCMD(0x30);     
  EPD_W21_WriteDATA(0x3C);    // PLL frame rate

  EPD_W21_WriteCMD(0x61);     // resolution setting
  EPD_W21_WriteDATA(EPD_WIDTH);       
  EPD_W21_WriteDATA(EPD_HEIGHT / 256);
  EPD_W21_WriteDATA(EPD_HEIGHT % 256); 

  EPD_W21_WriteCMD(0x82);     // vcom_DC setting    
  EPD_W21_WriteDATA(0x12); 
  lut1(); 

  EPD_W21_WriteCMD(0x04);     // power on
  lcd_chkstatus();
}

void EPD_DeepSleep(void)
{
  EPD_W21_WriteCMD(0x50);     // VCOM AND DATA INTERVAL SETTING     
  EPD_W21_WriteDATA(0xf7);    // WBmode

  EPD_W21_WriteCMD(0x02);     // power off
  lcd_chkstatus();
  delay(100);
  EPD_W21_WriteCMD(0x07);     // deep sleep
  EPD_W21_WriteDATA(0xA5);
}

// Full screen refresh update function
void EPD_Update(void)
{   
  EPD_W21_WriteCMD(0x12);     // DISPLAY REFRESH   
  delay(1);
  lcd_chkstatus();
}

void EPD_WhiteScreen_ALL(const unsigned char *datas)
{
  unsigned int i;
  EPD_W21_WriteCMD(0x10);     // Transfer old data
  for (i = 0; i < EPD_ARRAY; i++) { 
    EPD_W21_WriteDATA(0xFF);
  } 
  EPD_W21_WriteCMD(0x13);     // Transfer new data
  for (i = 0; i < EPD_ARRAY; i++) {
    EPD_W21_WriteDATA(pgm_read_byte(&datas[i]));
  }
  EPD_Update();     
}

void EPD_WhiteScreen_White(void)
{
  unsigned int i;
  EPD_W21_WriteCMD(0x10);     // Transfer old data
  for (i = 0; i < EPD_ARRAY; i++) { 
    EPD_W21_WriteDATA(0xFF); 
  }
  EPD_W21_WriteCMD(0x13);     // Transfer new data
  for (i = 0; i < EPD_ARRAY; i++) {
    EPD_W21_WriteDATA(0xFF);
    oldData[i] = 0xFF; 
  }
  EPD_Update();     
}

void EPD_WhiteScreen_Black(void)
{
  unsigned int i;
  EPD_W21_WriteCMD(0x10);
  for (i = 0; i < EPD_ARRAY; i++) { 
    EPD_W21_WriteDATA(0xFF); 
  }
  EPD_W21_WriteCMD(0x13);
  for (i = 0; i < EPD_ARRAY; i++) {
    EPD_W21_WriteDATA(0x00);
    oldData[i] = 0x00; 
  }
  EPD_Update();     
}

// Partial refresh base map background
void EPD_SetRAMValue_BaseMap(const unsigned char * datas)
{
  unsigned int i; 
  EPD_W21_WriteCMD(0x10);     // write old data 
  for (i = 0; i < EPD_ARRAY; i++) {               
    EPD_W21_WriteDATA(0xFF);
  }
  EPD_W21_WriteCMD(0x13);     // write new data 
  for (i = 0; i < EPD_ARRAY; i++) {               
    EPD_W21_WriteDATA(pgm_read_byte(&datas[i]));
    oldData[i] = pgm_read_byte(&datas[i]); 
  }   
  EPD_Update();     
}

void EPD_Dis_Part(unsigned int x_start, unsigned int y_start, const unsigned char * datas, unsigned int PART_COLUMN, unsigned int PART_LINE)
{
  unsigned int i, x_end, y_end;
  x_start = x_start - x_start % 8;
  x_end = x_start + PART_LINE - 1; 
  y_end = y_start + PART_COLUMN - 1;

  EPD_Init_Part();  
  EPD_W21_WriteCMD(0x91);     // partial mode in
  EPD_W21_WriteCMD(0x90);     // resolution setting
  EPD_W21_WriteDATA(x_start);      
  EPD_W21_WriteDATA(x_end - 1);     
  EPD_W21_WriteDATA(y_start / 256);
  EPD_W21_WriteDATA(y_start % 256);     
  EPD_W21_WriteDATA(y_end / 256);    
  EPD_W21_WriteDATA(y_end % 256 - 1);
  EPD_W21_WriteDATA(0x28);   

  EPD_W21_WriteCMD(0x10);     // writes Old data
  if (partFlag == 1) {
    partFlag = 0;
    for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++)       
      EPD_W21_WriteDATA(0xFF); 
  } else {
    for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++)       
      EPD_W21_WriteDATA(oldData[i]);   
  }

  EPD_W21_WriteCMD(0x13);     // writes New data
  for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++) {   
    EPD_W21_WriteDATA(pgm_read_byte(&datas[i])); 
    oldData[i] = pgm_read_byte(&datas[i]);     
  } 
  EPD_Update();   
}

void EPD_Dis_PartAll(const unsigned char * datas)
{
  unsigned int i;
  EPD_Init_Part();
  EPD_W21_WriteCMD(0x10);
  for (i = 0; i < EPD_ARRAY; i++) { 
    EPD_W21_WriteDATA(oldData[i]);
  } 
  EPD_W21_WriteCMD(0x13);
  for (i = 0; i < EPD_ARRAY; i++) {
    EPD_W21_WriteDATA(pgm_read_byte(&datas[i]));
    oldData[i] = pgm_read_byte(&datas[i]); 
  }  
  EPD_Update();      
}

void EPD_Dis_Part_RAM(unsigned int x_start, unsigned int y_start,
                      const unsigned char * datas_A, const unsigned char * datas_B,
                      const unsigned char * datas_C, const unsigned char * datas_D, const unsigned char * datas_E,
                      unsigned char num, unsigned int PART_COLUMN, unsigned int PART_LINE)
{
  unsigned int i, x_end, y_end;
  x_start = x_start - x_start % 8;
  x_end = x_start + PART_LINE - 1; 
  y_end = y_start + PART_COLUMN * num - 1;

  EPD_Init_Part();  
  EPD_W21_WriteCMD(0x91);
  EPD_W21_WriteCMD(0x90);
  EPD_W21_WriteDATA(x_start);     
  EPD_W21_WriteDATA(x_end - 1);     
  EPD_W21_WriteDATA(y_start / 256);
  EPD_W21_WriteDATA(y_start % 256);    
  EPD_W21_WriteDATA(y_end / 256);    
  EPD_W21_WriteDATA(y_end % 256 - 1);
  EPD_W21_WriteDATA(0x28); 

  EPD_W21_WriteCMD(0x10);
  if (partFlag == 1) {
    partFlag = 0;
    for (i = 0; i < PART_COLUMN * PART_LINE * num / 8; i++)       
      EPD_W21_WriteDATA(0xFF); 
  } else {
    for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++)       
      EPD_W21_WriteDATA(oldDataA[i]);              
    for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++)       
      EPD_W21_WriteDATA(oldDataB[i]);  
    for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++)       
      EPD_W21_WriteDATA(oldDataC[i]);              
    for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++)       
      EPD_W21_WriteDATA(oldDataD[i]);
    for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++)       
      EPD_W21_WriteDATA(oldDataE[i]);                    
  } 

  EPD_W21_WriteCMD(0x13);
  for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++) {     
    EPD_W21_WriteDATA(pgm_read_byte(&datas_A[i]));  
    oldDataA[i] = pgm_read_byte(&datas_A[i]);
  }         
  for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++) {     
    EPD_W21_WriteDATA(pgm_read_byte(&datas_B[i]));  
    oldDataB[i] = pgm_read_byte(&datas_B[i]);
  } 
  for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++) {     
    EPD_W21_WriteDATA(pgm_read_byte(&datas_C[i]));  
    oldDataC[i] = pgm_read_byte(&datas_C[i]);
  } 
  for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++) {     
    EPD_W21_WriteDATA(pgm_read_byte(&datas_D[i]));  
    oldDataD[i] = pgm_read_byte(&datas_D[i]);
  } 
  for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++) {     
    EPD_W21_WriteDATA(pgm_read_byte(&datas_E[i]));  
    oldDataE[i] = pgm_read_byte(&datas_E[i]); 
  } 
  EPD_Update();
}

void EPD_Dis_Part_Time(unsigned int x_start, unsigned int y_start,
                       const unsigned char * datas_A, const unsigned char * datas_B,
                       const unsigned char * datas_C, const unsigned char * datas_D, const unsigned char * datas_E,
                       unsigned char num, unsigned int PART_COLUMN, unsigned int PART_LINE)
{
  EPD_Dis_Part_RAM(x_start, y_start, datas_A, datas_B, datas_C, datas_D, datas_E, num, PART_COLUMN, PART_LINE);
}   

// Display rotation 180 degrees initialization
void EPD_Init_180(void)
{ 
  unsigned char i;
  for (i = 0; i < 3; i++)
  {
    EPD_W21_RST_0;
    delay(10);
    EPD_W21_RST_1;
    delay(10);
  } 
  lcd_chkstatus();

  EPD_W21_WriteCMD(0x00);     // panel setting
  EPD_W21_WriteDATA(0x13);    // 180-degree scan direction
  EPD_W21_WriteDATA(0x0D);  

  EPD_W21_WriteCMD(0x61);     // resolution setting
  EPD_W21_WriteDATA(EPD_WIDTH);       
  EPD_W21_WriteDATA(EPD_HEIGHT / 256);
  EPD_W21_WriteDATA(EPD_HEIGHT % 256); 

  EPD_W21_WriteCMD(0x04);     // power on
  lcd_chkstatus();

  EPD_W21_WriteCMD(0x50);     // VCOM AND DATA INTERVAL SETTING      
  EPD_W21_WriteDATA(0x97);
}
