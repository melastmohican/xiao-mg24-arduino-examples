#include "Display_EPD_W21.h"

// Active HIGH busy check on SSD1677: 1 = busy, 0 = idle/ready
void lcd_chkstatus(void)
{
  unsigned long start = millis();
  while (isEPD_W21_BUSY != 0)
  {
    if (millis() - start > 10000) {
      Serial.println(F("[GDEQ0426T82] BUSY wait timeout!"));
      break;
    }
    delay(1);
  }
}

void EPD_HW_Init(void)
{
  EPD_W21_RST_0;
  delay(10);
  EPD_W21_RST_1;
  delay(10);
  lcd_chkstatus();

  EPD_W21_WriteCMD(0x12); // Soft reset
  delay(10);
  lcd_chkstatus();

  EPD_W21_WriteCMD(0x18); // Temperature sensor
  EPD_W21_WriteDATA(0x80);

  EPD_W21_WriteCMD(0x0C); // Booster soft start
  EPD_W21_WriteDATA(0xAE);
  EPD_W21_WriteDATA(0xC7);
  EPD_W21_WriteDATA(0xC3);
  EPD_W21_WriteDATA(0xC0);
  EPD_W21_WriteDATA(0x80);

  EPD_W21_WriteCMD(0x01); // Driver output control
  EPD_W21_WriteDATA((EPD_HEIGHT - 1) % 256);
  EPD_W21_WriteDATA((EPD_HEIGHT - 1) / 256);
  EPD_W21_WriteDATA(0x02);

  EPD_W21_WriteCMD(0x3C); // Border waveform
  EPD_W21_WriteDATA(0x01);

  EPD_SetRamArea(0, 0, EPD_WIDTH, EPD_HEIGHT);
}

void EPD_SetRamArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  EPD_W21_WriteCMD(0x11); // Entry mode: x inc, y inc (0x03)
  EPD_W21_WriteDATA(0x03);

  EPD_W21_WriteCMD(0x44); // RAM X range
  EPD_W21_WriteDATA(x % 256);
  EPD_W21_WriteDATA(x / 256);
  EPD_W21_WriteDATA((x + w - 1) % 256);
  EPD_W21_WriteDATA((x + w - 1) / 256);

  EPD_W21_WriteCMD(0x45); // RAM Y range
  EPD_W21_WriteDATA(y % 256);
  EPD_W21_WriteDATA(y / 256);
  EPD_W21_WriteDATA((y + h - 1) % 256);
  EPD_W21_WriteDATA((y + h - 1) / 256);

  EPD_W21_WriteCMD(0x4E); // RAM X address counter
  EPD_W21_WriteDATA(x % 256);
  EPD_W21_WriteDATA(x / 256);

  EPD_W21_WriteCMD(0x4F); // RAM Y address counter
  EPD_W21_WriteDATA(y % 256);
  EPD_W21_WriteDATA(y / 256);
}

void EPD_WhiteScreen_White(void)
{
  EPD_SetRamArea(0, 0, EPD_WIDTH, EPD_HEIGHT);

  for (uint8_t c = 0; c < 2; c++) {
    EPD_W21_WriteCMD(c == 0 ? 0x24 : 0x26);
    EPD_W21_CS_0;
    EPD_W21_DC_1;
    for (uint32_t i = 0; i < EPD_ARRAY; i++) {
      SPI_Write(0xFF);
    }
    EPD_W21_CS_1;
  }

  EPD_W21_WriteCMD(0x21);
  EPD_W21_WriteDATA(0x40);
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteCMD(0x1A);
  EPD_W21_WriteDATA(0x5A);
  EPD_W21_WriteCMD(0x22);
  EPD_W21_WriteDATA(0xD7);
  EPD_W21_WriteCMD(0x20);
  lcd_chkstatus();
}

void EPD_Full_Update_From_Buffer(const unsigned char *full_buffer)
{
  EPD_SetRamArea(0, 0, EPD_WIDTH, EPD_HEIGHT);

  // Write both current (0x24) and differential previous (0x26)
  for (uint8_t c = 0; c < 2; c++) {
    EPD_W21_WriteCMD(c == 0 ? 0x24 : 0x26);
    EPD_W21_CS_0;
    EPD_W21_DC_1;
    for (uint32_t i = 0; i < EPD_ARRAY; i++) {
      SPI_Write(full_buffer[i]);
    }
    EPD_W21_CS_1;
  }

  // Fast full refresh
  EPD_W21_WriteCMD(0x21);
  EPD_W21_WriteDATA(0x40);
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteCMD(0x1A);
  EPD_W21_WriteDATA(0x5A);
  EPD_W21_WriteCMD(0x22);
  EPD_W21_WriteDATA(0xD7);
  EPD_W21_WriteCMD(0x20);
  lcd_chkstatus();
}

void EPD_Part_Update_From_Buffer(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const unsigned char *full_buffer)
{
  uint16_t startByteX = x / 8;
  uint16_t bytesPerRow = w / 8;

  EPD_SetRamArea(x, y, w, h);

  // Write window rows to 0x24
  EPD_W21_WriteCMD(0x24);
  EPD_W21_CS_0;
  EPD_W21_DC_1;
  for (uint16_t r = 0; r < h; r++) {
    uint32_t rowOffset = (uint32_t)(y + r) * (EPD_WIDTH / 8) + startByteX;
    for (uint16_t b = 0; b < bytesPerRow; b++) {
      SPI_Write(full_buffer[rowOffset + b]);
    }
  }
  EPD_W21_CS_1;

  // Differential partial refresh
  EPD_W21_WriteCMD(0x21);
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteCMD(0x22);
  EPD_W21_WriteDATA(0xFC);
  EPD_W21_WriteCMD(0x20);
  lcd_chkstatus();

  // Sync differential previous buffer 0x26
  EPD_W21_WriteCMD(0x26);
  EPD_W21_CS_0;
  EPD_W21_DC_1;
  for (uint16_t r = 0; r < h; r++) {
    uint32_t rowOffset = (uint32_t)(y + r) * (EPD_WIDTH / 8) + startByteX;
    for (uint16_t b = 0; b < bytesPerRow; b++) {
      SPI_Write(full_buffer[rowOffset + b]);
    }
  }
  EPD_W21_CS_1;
}

void EPD_DeepSleep(void)
{
  EPD_W21_WriteCMD(0x22);
  EPD_W21_WriteDATA(0x83);
  EPD_W21_WriteCMD(0x20);
  lcd_chkstatus();

  EPD_W21_WriteCMD(0x10);
  EPD_W21_WriteDATA(0x01);
}
