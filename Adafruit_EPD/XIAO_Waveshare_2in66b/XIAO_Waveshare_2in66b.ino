/*****************************************************************************
* | File        :   XIAO_Waveshare_2in66b.ino
* | Function    :   Drive the Waveshare 2.66inch e-Paper (B) with Adafruit_EPD
* | Info        :
*----------------
* The Waveshare 2.66" (B) panel (152x296 BWR) uses an SSD1680 controller, which
* Adafruit_EPD already drives at exactly this resolution as
* ThinkInk_266_Tricolor_MFGNR. Unlike the 3.52" panel in this repo, no local
* panel class is needed - this sketch is the stock class with the driver
* board's pins.
*
* Board : SiliconLabs:silabs:xiao_mg24:protocol_stack=none
* Panel : Waveshare 2.66inch e-Paper (B), 152x296, SSD1680, 24-pin FPC connector
* Driver: Seeed Studio ePaper Driver Board for XIAO v2
*         https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
*
* Ported from the Adafruit Feather RP2040 ThinkInk version in
* ../../../AdafruitFeatherThinkInk/. Only the pin block changed: the EPD is on
* the default SPI here (D8=SCK, D10=MOSI are the MG24's hardware SPI), not a
* secondary bus.
*
* MFGNR is the right class rather than a lucky guess: Waveshare's own product
* specification for this panel names the driver IC as SSD1680Z8 (mechanical
* drawing note, p6 of 2.66inch-e-paper-b-specification.pdf), and MFGNR is
* Adafruit's SSD1680Z variant class. The same page gives the resolution as
* "296gate x 152source", which is why the constructor takes the long axis first.
*
* Based on Adafruit's ThinkInk_tricolor example.
******************************************************************************/
#include "Adafruit_ThinkInk.h"

// Seeed ePaper Driver Board for XIAO v2 wiring. Same mapping as the GxEPD2
// demos and the vendored Waveshare sketches in this repo.
#define EPD_RESET D0
#define EPD_CS    D1
#define EPD_BUSY  D2
#define EPD_DC    D3
#define SRAM_CS   -1   // no external SRAM on this board, use the MCU's
#define EPD_SPI   &SPI // D8=SCK / D10=MOSI already are the default SPI

// 152x296 tricolor, SSD1680. The class passes (296, 152) to Adafruit_SSD1680 -
// long axis first - so the constructor here takes only the pins.
ThinkInk_266_Tricolor_MFGNR display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY,
                                    EPD_SPI);

void setup() {
  Serial.begin(115200);
  // Serial is EUSART0 via the on-board CMSIS-DAP chip, not USB CDC, so bound
  // the wait - it must never block.
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  Serial.println("Waveshare 2.66in (B) on Adafruit_EPD / SSD1680");
  display.begin(THINKINK_TRICOLOR);

  // 180 degrees, matching the Waveshare_2in66br sketch's EPD_2IN66B_ROTATE_180:
  // this panel's native origin is the corner opposite the one you want content
  // to start from. That is a panel/FPC fact, not a board one, so it carries
  // over from the ThinkInk connector unchanged. Has to come after begin(),
  // which sets rotation 0 itself. Rotation 2 is even, so width()/height() stay
  // 296x152 and the layout below is unaffected.
  display.setRotation(2);

  Serial.printf("Panel reports %d x %d\r\n", display.width(), display.height());

  // Single-shot, not a loop: e-paper wants updating rarely, so two refreshes
  // and stop rather than cycling forever.
  Serial.println("Banner demo");
  display.clearBuffer();
  display.setTextSize(2);
  display.setCursor((display.width() - 96) / 2, (display.height() - 16) / 2);
  display.setTextColor(EPD_BLACK);
  display.print("Tri");
  display.setTextColor(EPD_RED);
  display.print("Color");
  display.display();

  delay(15000);

  // Thirds: white | black | red. Makes plane order and inversion obvious at a
  // glance - if black and red swap places, the buffer indices are crossed; if
  // white and black swap, it is the inversion flags.
  Serial.println("Color rectangle demo");
  display.clearBuffer();
  display.fillRect(display.width() / 3, 0, display.width() / 3,
                   display.height(), EPD_BLACK);
  display.fillRect((display.width() * 2) / 3, 0, display.width() / 3,
                   display.height(), EPD_RED);
  display.display();

  Serial.println("Done - panel left on the rectangle test pattern.");
}

void loop() {}
