#ifndef _DISPLAY_EPD_W21_SPI_
#define _DISPLAY_EPD_W21_SPI_

#include <Arduino.h>
#include <SPI.h>

// Pin mapping for Seeed Studio XIAO MG24 + Good Display DESPI-C102
#define EPD_RST_PIN   D0
#define EPD_CS_PIN    D1
#define EPD_BUSY_PIN  D2   // Active LOW: 0 = busy, 1 = ready/idle
#define EPD_DC_PIN    D3
#define EPD_SCK_PIN   D8   // Hardware SPI SCK
#define EPD_MOSI_PIN  D10  // Hardware SPI MOSI

#define EPD_SPI_PORT SPI

#define isEPD_W21_BUSY digitalRead(EPD_BUSY_PIN)
#define EPD_W21_RST_0 digitalWrite(EPD_RST_PIN, LOW)
#define EPD_W21_RST_1 digitalWrite(EPD_RST_PIN, HIGH)
#define EPD_W21_DC_0  digitalWrite(EPD_DC_PIN, LOW)
#define EPD_W21_DC_1  digitalWrite(EPD_DC_PIN, HIGH)
#define EPD_W21_CS_0 digitalWrite(EPD_CS_PIN, LOW)
#define EPD_W21_CS_1 digitalWrite(EPD_CS_PIN, HIGH)

void SPI_Write(unsigned char value);
void EPD_W21_WriteDATA(unsigned char datas);
void EPD_W21_WriteCMD(unsigned char command);

#endif

