#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"

// Busy check with 10s timeout: UC8175 BUSY pin is active LOW (0 = busy, 1 = ready/idle).
void lcd_chkstatus(void)
{ 
  unsigned long start = millis();
  while (isEPD_W21_BUSY == 0)
  {
    if (millis() - start > 10000) {
      Serial.println(F("GDEW0102I4FC BUSY timeout!"));
      break;
    }
    delay(1);
  }  
}

// UC8175 initialization sequence
void EPD_Init(void)
{ 
  EPD_W21_RST_0;    // Module reset
  delay(10);        // At least 10ms delay 
  EPD_W21_RST_1;
  delay(10);
  lcd_chkstatus();

  EPD_W21_WriteCMD(0x00);      // Panel setting
  EPD_W21_WriteDATA(0x5F);     // LUT from OTP  

  EPD_W21_WriteCMD(0x2A);      // IC hidden instructions / LUT option
  EPD_W21_WriteDATA(0x00); 
  EPD_W21_WriteDATA(0x00);  

  EPD_W21_WriteCMD(0x04);      // Power on
  lcd_chkstatus();
  
  EPD_W21_WriteCMD(0x50);      // VCOM and data interval setting
  EPD_W21_WriteDATA(0x97);
}

void EPD_DeepSleep(void)
{
  EPD_W21_WriteCMD(0x50);      // VCOM and data interval setting
  EPD_W21_WriteDATA(0xF7);

  EPD_W21_WriteCMD(0x02);      // Power off
  lcd_chkstatus();
  delay(100);

  EPD_W21_WriteCMD(0x07);      // Deep sleep
  EPD_W21_WriteDATA(0xA5);
}

void EPD_Update(void)
{   
  EPD_W21_WriteCMD(0x12);      // Display refresh
  delay(1);
  lcd_chkstatus();
}

void EPD_WhiteScreen_ALL(const unsigned char *datas)
{
  unsigned int i;
  EPD_W21_WriteCMD(0x10);      // Transfer old data
  for (i = 0; i < EPD_ARRAY; i++) { 
    EPD_W21_WriteDATA(0xFF);
  } 
  EPD_W21_WriteCMD(0x13);      // Transfer new data
  for (i = 0; i < EPD_ARRAY; i++) {
    EPD_W21_WriteDATA(pgm_read_byte(&datas[i]));
  }
  EPD_Update();     
}

void EPD_WhiteScreen_White(void)
{
  unsigned int i;
  EPD_W21_WriteCMD(0x10);
  for (i = 0; i < EPD_ARRAY; i++) { 
    EPD_W21_WriteDATA(0xFF); 
  }
  EPD_W21_WriteCMD(0x13);
  for (i = 0; i < EPD_ARRAY; i++) {
    EPD_W21_WriteDATA(0xFF);
  }
  EPD_Update();     
}

void EPD_WhiteScreen_Black(void)
{
  unsigned int i;
  EPD_W21_WriteCMD(0x10);
  for (i = 0; i < EPD_ARRAY; i++) { 
    EPD_W21_WriteDATA(0x00); 
  }
  EPD_W21_WriteCMD(0x13);
  for (i = 0; i < EPD_ARRAY; i++) {
    EPD_W21_WriteDATA(0x00);
  }
  EPD_Update();     
}

void EPD_Init_180(void)
{ 
  EPD_W21_RST_0;
  delay(10);
  EPD_W21_RST_1;
  delay(10);
  lcd_chkstatus();

  EPD_W21_WriteCMD(0x00);      // Panel setting
  EPD_W21_WriteDATA(0x53);     // 180 mirror

  EPD_W21_WriteCMD(0x2A);
  EPD_W21_WriteDATA(0x00); 
  EPD_W21_WriteDATA(0x00);  

  EPD_W21_WriteCMD(0x04);
  lcd_chkstatus();
  
  EPD_W21_WriteCMD(0x50);
  EPD_W21_WriteDATA(0x97);
}

