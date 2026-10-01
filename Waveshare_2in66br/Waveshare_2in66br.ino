/*****************************************************************************
* | File        :   Waveshare_2in66br.ino
* | Function    :   2.66inch e-Paper (B) demo - 152x296, black/white/red
* | Info        :
*----------------
* Adapted from Waveshare's original epd2in66b.ino to run on the Seeed Studio
* XIAO MG24 with the panel seated in the Seeed Studio ePaper Driver Board for
* XIAO v2's 24-pin FPC connector.
*
* Board       :   SiliconLabs:silabs:xiao_mg24:protocol_stack=none
* Panel       :   Waveshare 2.66inch e-Paper (B), 152x296 BWR, SSD1680
* Connection  :   Seeed ePaper Driver Board for XIAO v2, 24-pin FPC connector
*                 (the board hard-wires the EPD signals, nothing is jumper-wired)
*                 https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
*
* Ported from the Adafruit Feather RP2040 ThinkInk version in
* ../../AdafruitFeatherThinkInk/. The panel-level facts carried over unchanged;
* only epdif.h/.cpp is board-specific.
*
* Changes from the Waveshare original:
*  - epdif.h/.cpp: pins switched to the driver board's XIAO pins (RST D0, CS D1,
*    BUSY D2, DC D3, SCK D8, MOSI D10) on the default SPI - D8/D10 already are
*    the MG24's hardware SPI, and this core has no setSCK()/setTX() to remap
*    with. IfInit() opens exactly one SPI transaction and never closes it:
*    SilabsSPI::beginTransaction() takes a FreeRTOS mutex with portMAX_DELAY and
*    returns still holding it, so a second call deadlocks. PWR_PIN dropped: the
*    driver board has no software power gate, so there is nothing to switch on.
*  - epd2in66b.cpp: `static` removed from the five out-of-class member
*    definitions - the vendor copy does not compile as shipped. WaitUntilIdle()
*    gained a 40s timeout so a stalled panel fails loudly instead of hanging
*    forever, and Clear() no longer runs one row past the RAM window.
*    Sleep() is unchanged: SSD1680 deep-sleeps straight from 0x10, unlike the
*    UC8253 3.52" panel in this repo, which wants POWER_OFF first.
*    DisplayFrame() rotates the bitmap 180 degrees (EPD_2IN66B_ROTATE_180): the
*    demo images are authored for Waveshare's own driver board, and in this
*    connector they otherwise land with the logo bottom-right instead of
*    top-left. Same class of fix as the 3.52" sketch's Paint rotation, which
*    also exists only to match the built-in artwork's orientation.
*  - This file: a bounded wait on Serial before printing, or the banner and the
*    init result go nowhere. Serial here is EUSART0 bridged through the on-board
*    CMSIS-DAP chip rather than a USB CDC endpoint, so the wait must never block
*    forever. The closing Clear() is disabled so the demo ends on the image
*    instead of a blank panel.
*
* Everything else is Waveshare's, unchanged; their licence below applies.
*
    @filename   :   epd2in66b.ino
    @brief      :   2.66inch b e-paper display demo
    @author     :   Waveshare

    Copyright (C) Waveshare     Dec 02 2020

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documnetation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to  whom the Software is
   furished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/

#include <SPI.h>
#include "epd2in66b.h"
#include "imagedata.h"
#include "epdpaint.h"

#define COLORED     0
#define UNCOLORED   1

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  // Serial is EUSART0 via the on-board CMSIS-DAP chip, not USB CDC, so bound
  // the wait - it must never block.
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  Epd epd;
  if (epd.Init() != 0) {
    Serial.print("e-Paper init failed...");
    return;
  }
  Serial.print("2.66inch b e-Paper demo...\r\n ");
  Serial.print("e-Paper Clear...\r\n ");
  epd.Clear();  
  
  Serial.print("draw image...\r\n ");
  epd.DisplayFrame(gImage_2in66bb, gImage_2in66br);
  delay(4000);

  // The wipe-to-white is left off deliberately: ending on the drawn image rather
  // than a blank panel. A cleared BWR panel looks grey next to a mono one - that
  // is the panel's white point, not a fault. Flip to 1 for Waveshare's behaviour.
#if 0
  Serial.print("clear......\r\n ");
  epd.Clear();
#endif

  Serial.print("sleep......\r\n ");
  epd.Sleep();
}

void loop() {
  // put your main code here, to run repeatedly:

}
