/*****************************************************************************
* | File        :   GDEY029F51H.ino
* | Function    :   2.9inch 4-color e-Paper demo, 168x384 (portrait), Black/White/Yellow/Red
* | Info        :
*----------------
* Good Display official GDEY029F51H sample (AU-GDEY029F51H-20230712, JD79667
* controller), adapted to run on the Seeed Studio XIAO MG24 with the panel
* seated in the Seeed Studio ePaper Driver Board for XIAO v2 24-pin FPC connector.
*
* NOT the same panel as GDEY029F52 (296x128, JD79661): this one is 168x384.
* The two share most init commands but differ in TRES (0x61) and RAM size
* (16,128 bytes here vs 9,472 for the F52).
*
* STATUS: vendor port, NOT verified on a real GDEY029F51H. It was run once on a
* unit that was ordered as an F51H but is actually a GDEY029F52 (sticker
* 029SW-F52-B2, R70H probe 83 02 01 = JD79661). On that unit every BUSY wait runs
* into the 40 s timeout ("GDEY029F51H BUSY timeout!" on Serial) and nothing is
* drawn: the F51H init sets TRES to 168x384, which this JD79661 panel does not
* complete. Use good_display/GDEY029F52/ for that panel, and
* epd_diag_jd7966x_rev/ to read which controller a unit has.
*
* Board       :   SiliconLabs:silabs:xiao_mg24:protocol_stack=none
* Panel       :   GDEY029F51H, 168x384, JD79667 controller
* Connection  :   Seeed ePaper Driver Board for XIAO v2, 24-pin FPC connector
*                 https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
*
* Pinout:
*   RST  -> D0
*   CS   -> D1
*   BUSY -> D2  (Active LOW on JD79667: 0 = busy, 1 = ready/idle)
*   DC   -> D3
*   SCK  -> D8
*   MOSI -> D10
*
* Changes from the Good Display original:
*  - Display_EPD_W21_spi.h/.cpp: pins mapped to Seeed driver board XIAO pins.
*  - Display_EPD_W21.cpp: busy checking uses a 40s timeout so a disconnected or
*    stalled panel fails cleanly instead of hanging forever. The init, sleep,
*    fill and image functions are the vendor's, unchanged.
*  - SPI transaction is opened once with 4 MHz clock and left open, avoiding
*    Silabs FreeRTOS mutex deadlock.
*  - The vendor's single-color fill tests (disabled by default in the original)
*    are enabled, as in the other 4-color Good Display ports in this repo.
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
  Serial.println(F(" GDEY029F51H (Good Display) Demo on XIAO MG24    "));
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
  // 1. Full screen update with the official Good Display 4-color bitmap
  Serial.println(F("1. Full Screen Update (gImage_1, ~16s)..."));
  EPD_init();
  PIC_display(gImage_1);
  EPD_sleep();
  Serial.println(F("   Holding display for 3s..."));
  delay(3000);

  // 2. Single color fill tests
  Serial.println(F("2. Single color fill tests..."));
  Serial.println(F("   - Black fill"));
  EPD_init();
  Display_All_Black();
  EPD_sleep();
  delay(3000);

  Serial.println(F("   - Yellow fill"));
  EPD_init();
  Display_All_Yellow();
  EPD_sleep();
  delay(3000);

  Serial.println(F("   - Red fill"));
  EPD_init();
  Display_All_Red();
  EPD_sleep();
  delay(3000);

  Serial.println(F("   - White clear"));
  EPD_init();
  Display_All_White();
  EPD_sleep();

  Serial.println(F("Demo sequence complete. Program halted."));
  while (1) {
    delay(1000);
  }
}
