#ifndef _DISPLAY_EPD_W21_SPI_
#define _DISPLAY_EPD_W21_SPI_
#include "Arduino.h"
#include <SPI.h>

// IO settings
// Seeed Studio ePaper Driver Board for XIAO v2:
//   https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
// RST  -> D0
// CS   -> D1
// BUSY -> D2 (Active HIGH on SSD1683: 1 = busy, 0 = idle)
// DC   -> D3
// SCK  -> D8
// MOSI -> D10
#define EPD_RST_PIN   D0
#define EPD_CS_PIN    D1
#define EPD_BUSY_PIN  D2
#define EPD_DC_PIN    D3
#define EPD_SCK_PIN   D8
#define EPD_MOSI_PIN  D10

#define EPD_SPI_PORT SPI

#define isEPD_W21_BUSY digitalRead(EPD_BUSY_PIN)
#define EPD_W21_RST_0 digitalWrite(EPD_RST_PIN, LOW)
#define EPD_W21_RST_1 digitalWrite(EPD_RST_PIN, HIGH)
#define EPD_W21_DC_0  digitalWrite(EPD_DC_PIN, LOW)
#define EPD_W21_DC_1  digitalWrite(EPD_DC_PIN, HIGH)
#define EPD_W21_CS_0  digitalWrite(EPD_CS_PIN, LOW)
#define EPD_W21_CS_1  digitalWrite(EPD_CS_PIN, HIGH)

void SPI_Write(unsigned char value);
void EPD_W21_WriteDATA(unsigned char datas);
void EPD_W21_WriteCMD(unsigned char command);

#endif

