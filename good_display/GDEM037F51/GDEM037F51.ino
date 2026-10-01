/*****************************************************************************
* | File        :   GDEM037F51.ino
* | Function    :   3.7inch 4-color e-Paper demo - 240x416, Black/White/Yellow/Red
* | Info        :
*----------------
* Good Display official sample for GDEM037F51 (IST7163 controller), adapted to
* run on the Seeed Studio XIAO MG24 with the panel seated in the Seeed Studio
* ePaper Driver Board for XIAO v2's 24-pin FPC connector.
*
* Board       :   SiliconLabs:silabs:xiao_mg24:protocol_stack=none
* Panel       :   GDEM037F51 / Waveshare 3.7inch e-Paper (G), 240x416, IST7163
* Connection  :   Seeed ePaper Driver Board for XIAO v2, 24-pin FPC connector
*                 https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
*
* Pinout:
*   RST  -> D0
*   CS   -> D1
*   BUSY -> D2  (Active LOW on IST7163)
*   DC   -> D3
*   SCK  -> D8
*   MOSI -> D10
*
* Changes from the Good Display original:
*  - Display_EPD_W21_spi.h/.cpp: pins mapped to Seeed driver board XIAO pins.
*  - Display_EPD_W21.cpp: busy checking uses a 40s timeout so a disconnected or
*    stalled panel fails cleanly instead of hanging forever.
*  - SPI transaction is opened once with 4 MHz clock and left open, avoiding
*    Silabs FreeRTOS mutex deadlock.
*  - Loop halts unconditionally with while(1) after running the demo sequence.
******************************************************************************/
#include <SPI.h>
#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "Ap_29demo.h"

void setup()
{
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  Serial.println();
  Serial.println(F("=================================================="));
  Serial.println(F(" GDEM037F51 (Good Display) Demo on XIAO MG24      "));
  Serial.println(F("=================================================="));

  pinMode(EPD_BUSY_PIN, INPUT);
  pinMode(EPD_RST_PIN,  OUTPUT);
  pinMode(EPD_DC_PIN,   OUTPUT);
  pinMode(EPD_CS_PIN,   OUTPUT);

  // EPD SPI transaction opened once for sketch lifetime
  EPD_SPI_PORT.begin();
  EPD_SPI_PORT.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
}

void loop()
{
  // 1. Full screen update with 4-color bitmap
  Serial.println(F("1. Full Screen Update (gImage_1, ~20s)..."));
  EPD_init();
  PIC_display(gImage_1);
  EPD_sleep();
  Serial.println(F("   Holding display for 5s..."));
  delay(5000);

  // 2. Fast screen update mode (~12-15s)
  Serial.println(F("2. Fast Screen Update Mode (gImage_1)..."));
  EPD_init_Fast();
  PIC_display(gImage_1);
  EPD_sleep();
  Serial.println(F("   Holding display for 5s..."));
  delay(5000);

  // 3. Single color fill tests
  Serial.println(F("3. Single color fill tests..."));
  EPD_init();
  Display_All_Yellow();
  EPD_sleep();
  delay(3000);

  EPD_init();
  Display_All_White();
  EPD_sleep();
  delay(3000);

  Serial.println(F("Demo sequence complete. Program halted."));
  while (1) {
    delay(1000);
  }
}
