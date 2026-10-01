/*****************************************************************************
* | File        :   XIAO_Waveshare_2in66.ino
* | Function    :   Drive the Waveshare 2.66inch e-Paper with Adafruit_EPD
* | Info        :
*----------------
* The Waveshare 2.66" monochrome panel (152x296) uses an SSD1680 controller, which
* Adafruit_EPD drives as ThinkInk_266_Grayscale4_MFGN.
* This sketch demonstrates both 1-bit monochrome and 4-level grayscale modes
* on the Seeed Studio XIAO MG24 + Seeed ePaper Driver Board v2.
*
* Board : SiliconLabs:silabs:xiao_mg24:protocol_stack=none
* Panel : Waveshare 2.66inch e-Paper (monochrome), 152x296, SSD1680, 24-pin FPC
* Driver: Seeed Studio ePaper Driver Board for XIAO v2
*         https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
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

// 152x296 monochrome/grayscale, SSD1680. The class passes (296, 152) to Adafruit_SSD1680
// - long axis first - so the constructor here takes only the pins.
ThinkInk_266_Grayscale4_MFGN display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY,
                                    EPD_SPI);

void setup() {
  Serial.begin(115200);
  // Serial is EUSART0 via the on-board CMSIS-DAP chip, not USB CDC, so bound
  // the wait - it must never block.
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  Serial.println(F("Waveshare 2.66in (Mono/Gray4) on Adafruit_EPD / SSD1680"));

  // First pass: 4-level grayscale mode
  display.begin(THINKINK_GRAYSCALE4);

  // 180 degrees correction matching the other 2.66" sketches: this panel's
  // native origin is the corner opposite the one you want content to start from.
  // Has to come after begin(), which sets rotation 0 itself.
  display.setRotation(2);

  Serial.printf("Panel reports %d x %d\r\n", display.width(), display.height());

  // Screen 1: Banner & Typography
  Serial.println(F("Banner demo"));
  display.clearBuffer();
  display.setTextSize(2);
  display.setTextColor(EPD_BLACK);
  display.setCursor((display.width() - 216) / 2, 24);
  display.print("Waveshare 2.66\"");

  display.setTextSize(1);
  display.setTextColor(EPD_DARK);
  display.setCursor((display.width() - 156) / 2, 54);
  display.print("296x152 SSD1680 E-Ink");

  display.setTextColor(EPD_LIGHT);
  display.setCursor((display.width() - 174) / 2, 70);
  display.print("Seeed XIAO MG24 Sense");

  // 4-level grayscale bar at bottom
  int barW = display.width() / 4;
  int barY = 96;
  int barH = 40;
  display.fillRect(0 * barW, barY, barW, barH, EPD_BLACK);
  display.fillRect(1 * barW, barY, barW, barH, EPD_DARK);
  display.fillRect(2 * barW, barY, barW, barH, EPD_LIGHT);
  display.fillRect(3 * barW, barY, barW, barH, EPD_WHITE);
  display.drawRect(3 * barW, barY, barW, barH, EPD_BLACK);

  display.setTextColor(EPD_WHITE);
  display.setCursor(0 * barW + 10, barY + 16); display.print("Black");
  display.setCursor(1 * barW + 10, barY + 16); display.print("Dark");
  display.setTextColor(EPD_BLACK);
  display.setCursor(2 * barW + 10, barY + 16); display.print("Light");
  display.setCursor(3 * barW + 10, barY + 16); display.print("White");

  display.display();

  delay(10000);

  // Screen 2: Concentric geometry & test patterns
  Serial.println(F("Geometric grayscale test patterns"));
  display.clearBuffer();
  display.drawRect(0, 0, display.width(), display.height(), EPD_BLACK);

  // Concentric rounded rectangles with alternating gray shades
  int pad = 8;
  display.fillRoundRect(pad, pad, display.width() - 2 * pad, display.height() - 2 * pad, 8, EPD_LIGHT);
  pad += 10;
  display.fillRoundRect(pad, pad, display.width() - 2 * pad, display.height() - 2 * pad, 6, EPD_DARK);
  pad += 10;
  display.fillRoundRect(pad, pad, display.width() - 2 * pad, display.height() - 2 * pad, 4, EPD_BLACK);
  pad += 10;
  display.fillRoundRect(pad, pad, display.width() - 2 * pad, display.height() - 2 * pad, 4, EPD_WHITE);

  display.setTextColor(EPD_BLACK);
  display.setTextSize(2);
  display.setCursor((display.width() - 144) / 2, (display.height() - 16) / 2);
  display.print("4-Level Gray");

  display.display();

  Serial.println(F("Done - panel left on geometric test pattern."));
}

void loop() {}
