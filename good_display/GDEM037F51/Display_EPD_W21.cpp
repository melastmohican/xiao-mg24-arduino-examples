#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"

// Busy function with 40s timeout: IST7163 BUSY pin is active LOW (0 = busy, 1 = ready/idle).
void lcd_chkstatus(void)
{
  unsigned long start = millis();
  while (isEPD_W21_BUSY == 0)
  {
    if (millis() - start > 40000) {
      Serial.println(F("GDEM037F51 BUSY timeout!"));
      break;
    }
    delay(5);
  }
}

static void EPD_reset(void)
{
  delay(20);
  EPD_W21_RST_0;
  delay(2);
  EPD_W21_RST_1;
  delay(20);
}

void EPD_init(void)
{
  EPD_reset();
  lcd_chkstatus();

  EPD_W21_WriteCMD(0x00);
  EPD_W21_WriteDATA(0x0F);
  EPD_W21_WriteDATA(0x29);

  EPD_W21_WriteCMD(0x01);
  EPD_W21_WriteDATA(0x07);
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA(0x22);
  EPD_W21_WriteDATA(0x78);
  EPD_W21_WriteDATA(0x0A);
  EPD_W21_WriteDATA(0x22);

  EPD_W21_WriteCMD(0x03);
  EPD_W21_WriteDATA(0x10);
  EPD_W21_WriteDATA(0x54);
  EPD_W21_WriteDATA(0x44);

  EPD_W21_WriteCMD(0x06);
  EPD_W21_WriteDATA(0xC0);
  EPD_W21_WriteDATA(0xC0);
  EPD_W21_WriteDATA(0xC0);

  EPD_W21_WriteCMD(0x30);
  EPD_W21_WriteDATA(0x08);

  EPD_W21_WriteCMD(0x41);
  EPD_W21_WriteDATA(0x00);

  EPD_W21_WriteCMD(0x50);
  EPD_W21_WriteDATA(0x37);

  EPD_W21_WriteCMD(0x60);
  EPD_W21_WriteDATA(0x02);
  EPD_W21_WriteDATA(0x02);

  EPD_W21_WriteCMD(0x61);
  EPD_W21_WriteDATA(Source_BITS / 256);
  EPD_W21_WriteDATA(Source_BITS % 256);
  EPD_W21_WriteDATA(Gate_BITS / 256);
  EPD_W21_WriteDATA(Gate_BITS % 256);

  EPD_W21_WriteCMD(0x65);
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA(0x00);

  EPD_W21_WriteCMD(0xE7);
  EPD_W21_WriteDATA(0x1C);

  EPD_W21_WriteCMD(0xE3);
  EPD_W21_WriteDATA(0x22);

  EPD_W21_WriteCMD(0xFF);
  EPD_W21_WriteDATA(0xA5);

  EPD_W21_WriteCMD(0xEF);
  EPD_W21_WriteDATA(0x01);
  EPD_W21_WriteDATA(0x1E);
  EPD_W21_WriteDATA(0x0A);
  EPD_W21_WriteDATA(0x1B);
  EPD_W21_WriteDATA(0x0B);
  EPD_W21_WriteDATA(0x17);

  EPD_W21_WriteCMD(0xC3);
  EPD_W21_WriteDATA(0xFD);

  EPD_W21_WriteCMD(0xDC);
  EPD_W21_WriteDATA(0x01);

  EPD_W21_WriteCMD(0xDD);
  EPD_W21_WriteDATA(0x08);

  EPD_W21_WriteCMD(0xDE);
  EPD_W21_WriteDATA(0x41);

  EPD_W21_WriteCMD(0xFD);
  EPD_W21_WriteDATA(0x01);

  EPD_W21_WriteCMD(0xE8);
  EPD_W21_WriteDATA(0x03);

  EPD_W21_WriteCMD(0xDA);
  EPD_W21_WriteDATA(0x07);

  EPD_W21_WriteCMD(0xC9);
  EPD_W21_WriteDATA(0x00);

  EPD_W21_WriteCMD(0xA8);
  EPD_W21_WriteDATA(0x0F);

  EPD_W21_WriteCMD(0xFF);
  EPD_W21_WriteDATA(0xE3);

  EPD_W21_WriteCMD(0xE9);
  EPD_W21_WriteDATA(0x01);

  EPD_W21_WriteCMD(0x04); // Power on
  lcd_chkstatus();

  EPD_W21_WriteCMD(0xFF);
  EPD_W21_WriteDATA(0xA5);

  EPD_W21_WriteCMD(0xEF);
  EPD_W21_WriteDATA(0x03);
  EPD_W21_WriteDATA(0x1E);
  EPD_W21_WriteDATA(0x0A);
  EPD_W21_WriteDATA(0x1B);
  EPD_W21_WriteDATA(0x0E);
  EPD_W21_WriteDATA(0x15);

  EPD_W21_WriteCMD(0xDC);
  EPD_W21_WriteDATA(0x01);

  EPD_W21_WriteCMD(0xDD);
  EPD_W21_WriteDATA(0x08);

  EPD_W21_WriteCMD(0xDE);
  EPD_W21_WriteDATA(0x41);

  EPD_W21_WriteCMD(0xFF);
  EPD_W21_WriteDATA(0xE3);
}

void EPD_init_Fast(void)
{
  EPD_init();

  EPD_W21_WriteCMD(0xE0);
  EPD_W21_WriteDATA(0x02);

  EPD_W21_WriteCMD(0xE6);
  EPD_W21_WriteDATA(0x5B);

  EPD_W21_WriteCMD(0xA5);
  EPD_W21_WriteDATA(0x00);
  lcd_chkstatus();
}

void PIC_display(const unsigned char* picData)
{
  EPD_W21_WriteCMD(0x10);
  for (unsigned int i = 0; i < ALLSCREEN_BYTES; i++) {
    EPD_W21_WriteDATA(picData[i]);
  }

  EPD_W21_WriteCMD(0x12);
  EPD_W21_WriteDATA(0x00);
  lcd_chkstatus();
}

void Acep_color(unsigned char color)
{
  unsigned char byte_val = (color << 6) | (color << 4) | (color << 2) | color;
  EPD_W21_WriteCMD(0x10);
  for (unsigned int i = 0; i < ALLSCREEN_BYTES; i++) {
    EPD_W21_WriteDATA(byte_val);
  }

  EPD_W21_WriteCMD(0x12);
  EPD_W21_WriteDATA(0x00);
  lcd_chkstatus();
}

void Display_All_Black(void)  { Acep_color(black); }
void Display_All_White(void)  { Acep_color(white); }
void Display_All_Yellow(void) { Acep_color(yellow); }
void Display_All_Red(void)    { Acep_color(red); }

void EPD_sleep(void)
{
  EPD_W21_WriteCMD(0x02); // POWER_OFF
  EPD_W21_WriteDATA(0x00);
  lcd_chkstatus();
  EPD_W21_WriteCMD(0x07); // DEEP_SLEEP
  EPD_W21_WriteDATA(0xA5);
}
