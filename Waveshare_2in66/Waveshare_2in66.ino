/*****************************************************************************
* | File        :   Waveshare_2in66.ino
* | Function    :   2.66inch e-Paper demo - 152x296, black/white monochrome
* | Info        :
*----------------
* Adapted from Waveshare's original epd2in66.ino to run on the Seeed Studio
* XIAO MG24 with the panel seated in the Seeed Studio ePaper Driver Board for
* XIAO v2's 24-pin FPC connector.
*
* Board       :   SiliconLabs:silabs:xiao_mg24:protocol_stack=none
* Panel       :   Waveshare 2.66inch e-Paper (monochrome), 152x296, SSD1680
* Connection  :   Seeed ePaper Driver Board for XIAO v2, 24-pin FPC connector
*                 (the board hard-wires the EPD signals, nothing is jumper-wired)
*                 https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
*
* Changes from the Waveshare original:
*  - epdif.h/.cpp: pins switched to the driver board's XIAO pins (RST D0, CS D1,
*    BUSY D2, DC D3, SCK D8, MOSI D10) on the default SPI - D8/D10 already are
*    the MG24's hardware SPI, and this core has no setSCK()/setTX() to remap
*    with. IfInit() opens exactly one SPI transaction and never closes it:
*    SilabsSPI::beginTransaction() takes a FreeRTOS mutex with portMAX_DELAY and
*    returns still holding it, so a second call deadlocks. PWR_PIN dropped: the
*    driver board has no software power gate, so there is nothing to switch on.
*  - epd2in66.cpp: `static` removed from out-of-class member definitions - the
*    vendor copy does not compile as shipped in C++. WaitUntilIdle() gained a 40s
*    timeout so a stalled panel fails loudly instead of hanging forever, and
*    Clear() no longer runs one row past the RAM window.
*    DisplayFrame() rotates the bitmap 180 degrees (EPD_2IN66_ROTATE_180): the
*    demo image is authored for Waveshare's own driver board, and in this
*    connector it otherwise lands inverted.
*  - This file: a bounded wait on Serial before printing. Serial here is EUSART0
*    bridged through the on-board CMSIS-DAP chip rather than a USB CDC endpoint,
*    so the wait must never block forever.
*
* Everything else is Waveshare's, unchanged; their licence applies.
******************************************************************************/

#include <SPI.h>
#include "epd2in66.h"
#include "imagedata.h"
#include "epdpaint.h"

#define COLORED     0
#define UNCOLORED   1

UBYTE image[500];
Paint paint(image, 48, 80);    // width should be a multiple of 8

void setup() {
  Serial.begin(115200);
  // Serial is EUSART0 via the on-board CMSIS-DAP chip, not USB CDC, so bound
  // the wait - it must never block.
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  Serial.println(F("Waveshare 2.66in e-Paper on XIAO MG24 + ePaper Board v2"));

  Epd epd;
  if (epd.Init() != 0) {
    Serial.println(F("e-Paper init failed..."));
    return;
  }
  Serial.println(F("e-Paper Clear..."));
  epd.Clear();

  Serial.println(F("Draw image bitmap..."));
  epd.DisplayFrame(IMAGE_DATA);
  delay(4000);

  Serial.println(F("Partial refresh timer demo (01 -> 07 on top of image)..."));
  epd.Init_Partial();
  paint.SetRotate(ROTATE_270);

  for (UBYTE i = 1; i <= 7; i++) {
    char time_string[] = {'0', '0', ':', '0', (char)('0' + i), '\0'};

    paint.Clear(UNCOLORED);
    paint.DrawStringAt(10, 10, time_string, &Font16, COLORED);
    Serial.print(F("Partial update refresh: "));
    Serial.println(time_string);
    epd.DisplayFrame_part(paint.GetImage(), IMAGE_DATA, 20, 100, 48, 80);
    delay(500);
  }

  // The wipe-to-white is left off deliberately (matching Waveshare_2in66br):
  // ending on the drawn spec image rather than a blank panel. Flip to 1 for
  // Waveshare's original wipe behaviour.
#if 0
  Serial.println(F("e-Paper Clear..."));
  epd.Clear();
#endif

  Serial.println(F("Entering deep sleep..."));
  epd.Sleep();
}

void loop() {
  // Demo runs once in setup()
}
