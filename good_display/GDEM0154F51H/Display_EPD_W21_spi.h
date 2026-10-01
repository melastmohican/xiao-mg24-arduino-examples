#ifndef _DISPLAY_EPD_W21_SPI_
#define _DISPLAY_EPD_W21_SPI_
#include "Arduino.h"
#include <SPI.h>

//IO settings
// Seeed Studio ePaper Driver Board for XIAO v2: the panel sits in the board's
// 24-pin FPC connector and the board hard-wires the EPD signals to these XIAO
// pins, so there is nothing to wire. Same mapping as the GxEPD2 demos and the
// Waveshare sketches in this repo.
//   https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
#define EPD_RST_PIN   D0
#define EPD_CS_PIN    D1
#define EPD_BUSY_PIN  D2
#define EPD_DC_PIN    D3
#define EPD_SCK_PIN   D8    // PA3, the MG24's hardware SPI SCK
#define EPD_MOSI_PIN  D10   // PA5, the MG24's hardware SPI MOSI

// Default SPI already lands on D8/D10, so there is nothing to remap - and the
// Silicon Labs core has no setSCK()/setTX() equivalent anyway.
#define EPD_SPI_PORT SPI

#define isEPD_W21_BUSY digitalRead(EPD_BUSY_PIN)
#define EPD_W21_RST_0 digitalWrite(EPD_RST_PIN,LOW)
#define EPD_W21_RST_1 digitalWrite(EPD_RST_PIN,HIGH)
#define EPD_W21_DC_0  digitalWrite(EPD_DC_PIN,LOW)
#define EPD_W21_DC_1  digitalWrite(EPD_DC_PIN,HIGH)
#define EPD_W21_CS_0 digitalWrite(EPD_CS_PIN,LOW)
#define EPD_W21_CS_1 digitalWrite(EPD_CS_PIN,HIGH)

void SPI_Write(unsigned char value);
void EPD_W21_WriteDATA(unsigned char datas);
void EPD_W21_WriteCMD(unsigned char command);

#endif
