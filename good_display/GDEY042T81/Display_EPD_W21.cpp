#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"

//////////////////////////////////// E-paper demo //////////////////////////////////////////////////////////
// Busy function: SSD1683 is active HIGH (1 = busy, 0 = idle)
void Epaper_READBUSY(void)
{
  unsigned long start = millis();
  while (1)
  {
    if (isEPD_W21_BUSY == 0) break;
    if (millis() - start > 40000)
    {
      Serial.println(F("e-Paper BUSY timeout!"));
      return;
    }
    delay(1);
  }
}

// Full screen update initialization (Good Display default orientation)
void EPD_HW_Init(void)
{
  EPD_W21_RST_0;  // Module reset
  delay(10);
  EPD_W21_RST_1;
  delay(10);

  Epaper_READBUSY();
  EPD_W21_WriteCMD(0x12);  // SWRESET
  Epaper_READBUSY();

  EPD_W21_WriteCMD(0x01);  // Driver output control
  EPD_W21_WriteDATA((EPD_HEIGHT - 1) % 256);
  EPD_W21_WriteDATA((EPD_HEIGHT - 1) / 256);
  EPD_W21_WriteDATA(0x00);

  EPD_W21_WriteCMD(0x21);  // Display update control
  EPD_W21_WriteDATA(0x40);
  EPD_W21_WriteDATA(0x00);

  EPD_W21_WriteCMD(0x3C);  // BorderWaveform
  EPD_W21_WriteDATA(0x05);

  EPD_W21_WriteCMD(0x11);  // Data entry mode: 0x01 (X inc, Y dec)
  EPD_W21_WriteDATA(0x01);

  EPD_W21_WriteCMD(0x44);  // Set Ram-X address start/end position
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA(EPD_WIDTH / 8 - 1);

  EPD_W21_WriteCMD(0x45);  // Set Ram-Y address start/end position
  EPD_W21_WriteDATA((EPD_HEIGHT - 1) % 256);
  EPD_W21_WriteDATA((EPD_HEIGHT - 1) / 256);
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA(0x00);

  EPD_W21_WriteCMD(0x4E);  // Set RAM x address count to 0
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteCMD(0x4F);  // Set RAM y address count to (EPD_HEIGHT - 1)
  EPD_W21_WriteDATA((EPD_HEIGHT - 1) % 256);
  EPD_W21_WriteDATA((EPD_HEIGHT - 1) / 256);
  Epaper_READBUSY();
}

// 180-degree rotation init (Good Display rotated orientation)
void EPD_HW_Init_180(void)
{
  EPD_W21_RST_0;  // Module reset
  delay(10);
  EPD_W21_RST_1;
  delay(10);

  Epaper_READBUSY();
  EPD_W21_WriteCMD(0x12);  // SWRESET
  Epaper_READBUSY();

  EPD_W21_WriteCMD(0x21);  // Display update control
  EPD_W21_WriteDATA(0x40);
  EPD_W21_WriteDATA(0x00);

  EPD_W21_WriteCMD(0x3C);  // BorderWaveform
  EPD_W21_WriteDATA(0x05);

  EPD_W21_WriteCMD(0x11);  // Data entry mode: 0x02 (X dec, Y inc)
  EPD_W21_WriteDATA(0x02);

  EPD_W21_WriteCMD(0x44);  // Set Ram-X address start/end position
  EPD_W21_WriteDATA(EPD_WIDTH / 8 - 1);
  EPD_W21_WriteDATA(0x00);

  EPD_W21_WriteCMD(0x45);  // Set Ram-Y address start/end position
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA((EPD_HEIGHT - 1) % 256);
  EPD_W21_WriteDATA((EPD_HEIGHT - 1) / 256);

  EPD_W21_WriteCMD(0x4E);  // Set RAM x address count to (EPD_WIDTH/8 - 1)
  EPD_W21_WriteDATA(EPD_WIDTH / 8 - 1);
  EPD_W21_WriteCMD(0x4F);  // Set RAM y address count to 0
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA(0x00);
  Epaper_READBUSY();
}

// Fast update initialization (1.5s)
void EPD_HW_Init_Fast(void)
{
  EPD_W21_RST_0;  // Module reset
  delay(10);
  EPD_W21_RST_1;
  delay(10);

  EPD_W21_WriteCMD(0x12);  // SWRESET
  Epaper_READBUSY();

  EPD_W21_WriteCMD(0x21);
  EPD_W21_WriteDATA(0x40);
  EPD_W21_WriteDATA(0x00);

  EPD_W21_WriteCMD(0x3C);
  EPD_W21_WriteDATA(0x05);

  // 1.5s temperature waveform
  EPD_W21_WriteCMD(0x1A);
  EPD_W21_WriteDATA(0x6E);

  EPD_W21_WriteCMD(0x22);  // Load temperature value
  EPD_W21_WriteDATA(0x91);
  EPD_W21_WriteCMD(0x20);
  Epaper_READBUSY();

  EPD_W21_WriteCMD(0x11);  // Data entry mode: 0x01
  EPD_W21_WriteDATA(0x01);

  EPD_W21_WriteCMD(0x44);  // Set Ram-X address start/end position
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA(EPD_WIDTH / 8 - 1);

  EPD_W21_WriteCMD(0x45);  // Set Ram-Y address start/end position
  EPD_W21_WriteDATA((EPD_HEIGHT - 1) % 256);
  EPD_W21_WriteDATA((EPD_HEIGHT - 1) / 256);
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA(0x00);

  EPD_W21_WriteCMD(0x4E);  // Set RAM x address count to 0
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteCMD(0x4F);  // Set RAM y address count to (EPD_HEIGHT - 1)
  EPD_W21_WriteDATA((EPD_HEIGHT - 1) % 256);
  EPD_W21_WriteDATA((EPD_HEIGHT - 1) / 256);
  Epaper_READBUSY();
}

// 4 Gray update initialization
void EPD_HW_Init_4G(void)
{
  EPD_W21_RST_0;  // Module reset
  delay(10);
  EPD_W21_RST_1;
  delay(10);

  EPD_W21_WriteCMD(0x12);  // SWRESET
  Epaper_READBUSY();

  EPD_W21_WriteCMD(0x3C);
  EPD_W21_WriteDATA(0x05);

  // 4 Gray
  EPD_W21_WriteCMD(0x1A);
  EPD_W21_WriteDATA(0x5A);

  EPD_W21_WriteCMD(0x22);  // Load temperature value
  EPD_W21_WriteDATA(0x91);
  EPD_W21_WriteCMD(0x20);
  Epaper_READBUSY();

  EPD_W21_WriteCMD(0x11);  // Data entry mode: 0x01
  EPD_W21_WriteDATA(0x01);

  EPD_W21_WriteCMD(0x44);  // Set Ram-X address start/end position
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA(EPD_WIDTH / 8 - 1);

  EPD_W21_WriteCMD(0x45);  // Set Ram-Y address start/end position
  EPD_W21_WriteDATA((EPD_HEIGHT - 1) % 256);
  EPD_W21_WriteDATA((EPD_HEIGHT - 1) / 256);
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA(0x00);

  EPD_W21_WriteCMD(0x4E);  // Set RAM x address count to 0
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteCMD(0x4F);  // Set RAM y address count to (EPD_HEIGHT - 1)
  EPD_W21_WriteDATA((EPD_HEIGHT - 1) % 256);
  EPD_W21_WriteDATA((EPD_HEIGHT - 1) / 256);
  Epaper_READBUSY();
}

// GUI initialization
void EPD_HW_Init_GUI(void)
{
  EPD_W21_RST_0;  // Module reset
  delay(10);
  EPD_W21_RST_1;
  delay(10);

  Epaper_READBUSY();
  EPD_W21_WriteCMD(0x12);  // SWRESET
  Epaper_READBUSY();

  EPD_W21_WriteCMD(0x01);  // Driver output control
  EPD_W21_WriteDATA((EPD_HEIGHT - 1) % 256);
  EPD_W21_WriteDATA((EPD_HEIGHT - 1) / 256);
  EPD_W21_WriteDATA(0x00);

  EPD_W21_WriteCMD(0x21);  // Display update control
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA(0x00);

  EPD_W21_WriteCMD(0x3C);  // BorderWaveform
  EPD_W21_WriteDATA(0x05);

  EPD_W21_WriteCMD(0x11);  // Data entry mode: 0x03
  EPD_W21_WriteDATA(0x03);

  EPD_W21_WriteCMD(0x44);  // Set Ram-X address start/end position
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA(EPD_WIDTH / 8 - 1);

  EPD_W21_WriteCMD(0x45);  // Set Ram-Y address start/end position
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA((EPD_HEIGHT - 1) % 256);
  EPD_W21_WriteDATA((EPD_HEIGHT - 1) / 256);

  EPD_W21_WriteCMD(0x4E);  // Set RAM x address count to 0
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteCMD(0x4F);  // Set RAM y address count to 0
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA(0x00);
  Epaper_READBUSY();
}

////////////////////////////// Display Update Functions ///////////////////////////////////////////////////////
void EPD_Update(void)
{
  EPD_W21_WriteCMD(0x22);  // Display Update Control
  EPD_W21_WriteDATA(0xF7);
  EPD_W21_WriteCMD(0x20);  // Activate Display Update Sequence
  Epaper_READBUSY();
}

void EPD_Update_Fast(void)
{
  EPD_W21_WriteCMD(0x22);  // Display Update Control
  EPD_W21_WriteDATA(0xC7);
  EPD_W21_WriteCMD(0x20);  // Activate Display Update Sequence
  Epaper_READBUSY();
}

void EPD_Update_4G(void)
{
  EPD_W21_WriteCMD(0x22);  // Display Update Control
  EPD_W21_WriteDATA(0xCF);
  EPD_W21_WriteCMD(0x20);  // Activate Display Update Sequence
  Epaper_READBUSY();
}

void EPD_Part_Update(void)
{
  EPD_W21_WriteCMD(0x22);  // Display Update Control
  EPD_W21_WriteDATA(0xFF);
  EPD_W21_WriteCMD(0x20);  // Activate Display Update Sequence
  Epaper_READBUSY();
}

////////////////////////////// Display Data Transfer Functions //////////////////////////////////////////////
void EPD_WhiteScreen_ALL(const unsigned char *datas)
{
  unsigned int i;
  EPD_W21_WriteCMD(0x24);  // write RAM for black(0)/white (1)
  for (i = 0; i < EPD_ARRAY; i++)
  {
    EPD_W21_WriteDATA(pgm_read_byte(&datas[i]));
  }
  EPD_W21_WriteCMD(0x26);  // write RAM for black(0)/white (1)
  for (i = 0; i < EPD_ARRAY; i++)
  {
    EPD_W21_WriteDATA(pgm_read_byte(&datas[i]));
  }
  EPD_Update();
}

void EPD_WhiteScreen_ALL_Fast(const unsigned char *datas)
{
  unsigned int i;
  EPD_W21_WriteCMD(0x24);
  for (i = 0; i < EPD_ARRAY; i++)
  {
    EPD_W21_WriteDATA(pgm_read_byte(&datas[i]));
  }
  EPD_W21_WriteCMD(0x26);
  for (i = 0; i < EPD_ARRAY; i++)
  {
    EPD_W21_WriteDATA(pgm_read_byte(&datas[i]));
  }
  EPD_Update_Fast();
}

void EPD_WhiteScreen_White(void)
{
  unsigned int i;
  EPD_W21_WriteCMD(0x24);
  for (i = 0; i < EPD_ARRAY; i++)
  {
    EPD_W21_WriteDATA(0xFF);
  }
  EPD_W21_WriteCMD(0x26);
  for (i = 0; i < EPD_ARRAY; i++)
  {
    EPD_W21_WriteDATA(0xFF);
  }
  EPD_Update();
}

void EPD_WhiteScreen_Black(void)
{
  unsigned int i;
  EPD_W21_WriteCMD(0x24);
  for (i = 0; i < EPD_ARRAY; i++)
  {
    EPD_W21_WriteDATA(0x00);
  }
  EPD_W21_WriteCMD(0x26);
  for (i = 0; i < EPD_ARRAY; i++)
  {
    EPD_W21_WriteDATA(0x00);
  }
  EPD_Update();
}

void EPD_SetRAMValue_BaseMap(const unsigned char *datas)
{
  unsigned int i;
  EPD_W21_WriteCMD(0x24);
  for (i = 0; i < EPD_ARRAY; i++)
  {
    EPD_W21_WriteDATA(pgm_read_byte(&datas[i]));
  }
  EPD_W21_WriteCMD(0x26);
  for (i = 0; i < EPD_ARRAY; i++)
  {
    EPD_W21_WriteDATA(pgm_read_byte(&datas[i]));
  }
  EPD_Update();
}

void EPD_Dis_Part(unsigned int x_start, unsigned int y_start, const unsigned char *datas, unsigned int PART_COLUMN, unsigned int PART_LINE)
{
  unsigned int i;
  unsigned int x_end, y_end;

  x_start = x_start / 8;
  x_end = x_start + PART_LINE / 8 - 1;
  y_end = y_start + PART_COLUMN - 1;

  EPD_W21_RST_0;  // Module reset
  delay(10);
  EPD_W21_RST_1;
  delay(10);

  EPD_W21_WriteCMD(0x3C);  // BorderWaveform
  EPD_W21_WriteDATA(0x80);

  EPD_W21_WriteCMD(0x21);
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA(0x00);

  EPD_W21_WriteCMD(0x44);  // Set RAM x address start/end
  EPD_W21_WriteDATA(x_start);
  EPD_W21_WriteDATA(x_end);
  EPD_W21_WriteCMD(0x45);  // Set RAM y address start/end
  EPD_W21_WriteDATA(y_start % 256);
  EPD_W21_WriteDATA(y_start / 256);
  EPD_W21_WriteDATA(y_end % 256);
  EPD_W21_WriteDATA(y_end / 256);

  EPD_W21_WriteCMD(0x4E);  // Set RAM x address count to x_start
  EPD_W21_WriteDATA(x_start);
  EPD_W21_WriteCMD(0x4F);  // Set RAM y address count to y_start
  EPD_W21_WriteDATA(y_start % 256);
  EPD_W21_WriteDATA(y_start / 256);

  EPD_W21_WriteCMD(0x24);  // Write Black and White image to RAM
  for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++)
  {
    EPD_W21_WriteDATA(pgm_read_byte(&datas[i]));
  }
  EPD_Part_Update();
}

void EPD_Dis_PartAll(const unsigned char *datas)
{
  unsigned int i;
  unsigned int PART_COLUMN = EPD_HEIGHT, PART_LINE = EPD_WIDTH;

  EPD_W21_RST_0;  // Module reset
  delay(10);
  EPD_W21_RST_1;
  delay(10);

  EPD_W21_WriteCMD(0x3C);
  EPD_W21_WriteDATA(0x80);

  EPD_W21_WriteCMD(0x24);
  for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++)
  {
    EPD_W21_WriteDATA(pgm_read_byte(&datas[i]));
  }
  EPD_Part_Update();
}

// Write one partial-window tile into RAM without refreshing (vendor EPD_Dis_Part_RAM).
// EPD_Dis_Part_Time() writes several tiles, then refreshes once.
void EPD_Dis_Part_RAM(unsigned int x_start, unsigned int y_start, const unsigned char *datas, unsigned int PART_COLUMN, unsigned int PART_LINE)
{
  unsigned int i;
  unsigned int x_end, y_end;

  x_start = x_start / 8;                 // x address start
  x_end = x_start + PART_LINE / 8 - 1;   // x address end

  y_start = y_start - 1;                 // y address start (vendor offset)
  y_end = y_start + PART_COLUMN - 1;     // y address end

  EPD_W21_RST_0;  // Module reset
  delay(10);
  EPD_W21_RST_1;
  delay(10);

  EPD_W21_WriteCMD(0x21);
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA(0x00);

  EPD_W21_WriteCMD(0x3C);  // BorderWaveform
  EPD_W21_WriteDATA(0x80);

  EPD_W21_WriteCMD(0x44);  // Set RAM x address start/end
  EPD_W21_WriteDATA(x_start);
  EPD_W21_WriteDATA(x_end);
  EPD_W21_WriteCMD(0x45);  // Set RAM y address start/end
  EPD_W21_WriteDATA(y_start % 256);
  EPD_W21_WriteDATA(y_start / 256);
  EPD_W21_WriteDATA(y_end % 256);
  EPD_W21_WriteDATA(y_end / 256);

  EPD_W21_WriteCMD(0x4E);  // Set RAM x address count
  EPD_W21_WriteDATA(x_start);
  EPD_W21_WriteCMD(0x4F);  // Set RAM y address count
  EPD_W21_WriteDATA(y_start % 256);
  EPD_W21_WriteDATA(y_start / 256);

  EPD_W21_WriteCMD(0x24);  // Write black/white image to RAM
  for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++)
  {
    EPD_W21_WriteDATA(pgm_read_byte(&datas[i]));
  }
}

void EPD_Dis_Part_Time(unsigned int x_startA, unsigned int y_startA, const unsigned char *datasA,
                       unsigned int x_startB, unsigned int y_startB, const unsigned char *datasB,
                       unsigned int x_startC, unsigned int y_startC, const unsigned char *datasC,
                       unsigned int x_startD, unsigned int y_startD, const unsigned char *datasD,
                       unsigned int x_startE, unsigned int y_startE, const unsigned char *datasE,
                       unsigned int PART_COLUMN, unsigned int PART_LINE)
{
  EPD_Dis_Part_RAM(x_startA, y_startA, datasA, PART_COLUMN, PART_LINE);
  EPD_Dis_Part_RAM(x_startB, y_startB, datasB, PART_COLUMN, PART_LINE);
  EPD_Dis_Part_RAM(x_startC, y_startC, datasC, PART_COLUMN, PART_LINE);
  EPD_Dis_Part_RAM(x_startD, y_startD, datasD, PART_COLUMN, PART_LINE);
  EPD_Dis_Part_RAM(x_startE, y_startE, datasE, PART_COLUMN, PART_LINE);
  EPD_Part_Update();  // one refresh for all five tiles
}

// 4 Gray conversions
static unsigned char R24_DTM1(unsigned char Image_data1, unsigned char Image_data2)
{
  unsigned int i, all;
  unsigned char TempData1 = Image_data1, TempData2 = Image_data2;
  unsigned char outdata = 0x00;

  for (i = 0; i < 4; i++)
  {
    outdata = outdata << 1;
    if (((TempData1 & 0xC0) == 0xC0) || ((TempData1 & 0xC0) == 0x40))
      outdata |= 0x01;
    TempData1 <<= 2;
  }
  for (all = 0; all < 4; all++)
  {
    outdata = outdata << 1;
    if (((TempData2 & 0xC0) == 0xC0) || ((TempData2 & 0xC0) == 0x40))
      outdata |= 0x01;
    TempData2 <<= 2;
  }
  return outdata;
}

static unsigned char R26_DTM2(unsigned char Image_data1, unsigned char Image_data2)
{
  unsigned int i, all;
  unsigned char TempData1 = Image_data1, TempData2 = Image_data2;
  unsigned char outdata = 0x00;

  for (i = 0; i < 4; i++)
  {
    outdata = outdata << 1;
    if (((TempData1 & 0xC0) == 0xC0) || ((TempData1 & 0xC0) == 0x80))
      outdata |= 0x01;
    TempData1 <<= 2;
  }
  for (all = 0; all < 4; all++)
  {
    outdata = outdata << 1;
    if (((TempData2 & 0xC0) == 0xC0) || ((TempData2 & 0xC0) == 0x80))
      outdata |= 0x01;
    TempData2 <<= 2;
  }
  return outdata;
}

void EPD_WhiteScreen_ALL_4G(const unsigned char *datas)
{
  unsigned int i;
  unsigned char value;
  EPD_W21_WriteCMD(0x24);
  for (i = 0; i < EPD_ARRAY * 2; i += 2)
  {
    value = R24_DTM1(pgm_read_byte(datas + i), pgm_read_byte(datas + i + 1));
    EPD_W21_WriteDATA(value);
  }
  EPD_W21_WriteCMD(0x26);
  for (i = 0; i < EPD_ARRAY * 2; i += 2)
  {
    value = R26_DTM2(pgm_read_byte(datas + i), pgm_read_byte(datas + i + 1));
    EPD_W21_WriteDATA(value);
  }
  EPD_Update_4G();
}

void EPD_DeepSleep(void)
{
  EPD_W21_WriteCMD(0x10);  // Enter deep sleep
  EPD_W21_WriteDATA(0x01);
  delay(100);
}
