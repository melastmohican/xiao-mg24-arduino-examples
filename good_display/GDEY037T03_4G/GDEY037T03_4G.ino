/*****************************************************************************
* | File        :   GDEY037T03_4G.ino
* | Function    :   3.7inch e-Paper 4-gray test - 240x416, UC8253
* | Info        :
*----------------
* Good Display's official GDEY037T03-FP4G sample (A32-GDEY037T03-FP4G-20240815),
* adapted to run on the Seeed Studio XIAO MG24 with the panel seated in the Seeed
* Studio ePaper Driver Board for XIAO v2's 24-pin FPC connector.
*
* Purpose     :   independent check, outside epdsi, of whether a given GDEY037T03
*                 glass renders four gray levels. Display_EPD_W21.cpp and the image
*                 data are Good Display's, unmodified.
*
* Board       :   SiliconLabs:silabs:xiao_mg24:protocol_stack=none
* Panel       :   GDEY037T03 (3.7" 240x416, UC8253)
* Connection  :   Seeed ePaper Driver Board for XIAO v2, 24-pin FPC connector
*
* Pinout:
*   RST  -> D0
*   CS   -> D1
*   BUSY -> D2  (Active LOW on UC8253: 0 = busy, 1 = ready)
*   DC   -> D3
*   SCK  -> D8
*   MOSI -> D10
*
* Sequence    :   clear, monochrome image, then the 4-gray image. Photograph the
*                 panel after each step. A real 4-gray panel shows smooth gray
*                 steps on the last image; a 2-level panel shows only black and white.
******************************************************************************/
#include <SPI.h>
#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "Ap_29demo.h"

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  Serial.println(F("GDEY037T03 4-gray test (Good Display sample) on XIAO MG24 + ePaper Board v2"));

  pinMode(EPD_BUSY_PIN, INPUT);
  pinMode(EPD_RST_PIN,  OUTPUT);
  pinMode(EPD_DC_PIN,   OUTPUT);
  pinMode(EPD_CS_PIN,   OUTPUT);

  EPD_SPI_PORT.begin();
  EPD_SPI_PORT.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
}

void loop() {
  Serial.println(F("1. Clear (white)..."));
  EPD_Init();
  EPD_WhiteScreen_White();
  EPD_DeepSleep();
  delay(3000);

  Serial.println(F("2. Monochrome image (gImage_1)..."));
  EPD_Init();
  EPD_WhiteScreen_ALL(gImage_1);
  EPD_DeepSleep();
  delay(8000);

  Serial.println(F("3. 4-gray image (gImage_4G1)..."));
  EPD_Init_4G();
  EPD_WhiteScreen_ALL_4G(gImage_4G1);
  EPD_DeepSleep();

  Serial.println(F("Done. Halted."));
  while (1) {
    delay(1000);
  }
}
