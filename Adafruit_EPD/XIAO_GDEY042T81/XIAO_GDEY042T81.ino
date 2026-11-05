/*****************************************************************************
* | File        :   XIAO_GDEY042T81.ino
* | Function    :   Drive the GDEY042T81 / Waveshare 4.2" e-Paper with Adafruit_EPD
* | Info        :
*----------------
* The Good Display GDEY042T81 / Waveshare 4.2inch e-Paper V2 (400x300) uses an
* SSD1683 controller, supported natively by Adafruit_EPD as
* ThinkInk_420_Grayscale4_MFGN.
* This sketch demonstrates the 4-level grayscale mode (THINKINK_GRAYSCALE4) on
* the Seeed Studio XIAO MG24 + Seeed ePaper Driver Board v2. The class also
* supports 1-bit monochrome via THINKINK_MONO.
*
* Board : SiliconLabs:silabs:xiao_mg24:protocol_stack=none
* Panel : GDEY042T81 / Waveshare 4.2" V2, 400x300, SSD1683, 24-pin FPC
* Driver: Seeed Studio ePaper Driver Board for XIAO v2
*         https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
******************************************************************************/
#include "Adafruit_ThinkInk.h"

// Seeed ePaper Driver Board for XIAO v2 wiring. Same mapping as the GxEPD2
// demos and the other sketches in this repo.
#define EPD_RESET D0
#define EPD_CS    D1
#define EPD_BUSY  D2    // Active HIGH on SSD1683
#define EPD_DC    D3
#define SRAM_CS   -1   // No external SRAM, use the MCU's internal RAM
#define EPD_SPI   &SPI // D8=SCK / D10=MOSI default SPI

// 400x300 monochrome/grayscale, SSD1683.
// The class passes (300, 400) to Adafruit_SSD1683 and defaults to rotation 1 (400x300 landscape).
ThinkInk_420_Grayscale4_MFGN display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY,
                                     EPD_SPI);

// Print text horizontally centered on the display at the given top-left y,
// using the current text size (measured, not hard-coded).
void printCentered(const char* text, int16_t y) {
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, y, &x1, &y1, &w, &h);
  display.setCursor((display.width() - w) / 2 - x1, y);
  display.print(text);
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  Serial.println(F("GDEY042T81 / Waveshare 4.2\" on Adafruit_EPD / SSD1683"));

  // First pass: 4-level grayscale mode
  display.begin(THINKINK_GRAYSCALE4);

  Serial.printf("Panel reports %d x %d\r\n", display.width(), display.height());

  // Screen 1: Banner & Typography & 4-level gray gradient swatches
  Serial.println(F("Screen 1: Banner & 4-level grayscale palette"));
  display.clearBuffer();

  // Top header banner
  display.fillRect(0, 0, display.width(), 36, EPD_BLACK);
  display.setTextSize(2);
  display.setTextColor(EPD_WHITE);
  printCentered("GDEY042T81 / 4.2\" E-Paper", 10);

  display.setTextSize(2);
  display.setTextColor(EPD_BLACK);
  printCentered("400x300 SSD1683 Controller", 55);

  display.setTextSize(1);
  display.setTextColor(EPD_DARK);
  printCentered("Seeed Studio XIAO MG24 + ePaper Board v2", 85);

  display.setTextColor(EPD_BLACK);
  printCentered("4-Level Grayscale Mode Demonstration", 105);

  // Divider line
  display.drawFastHLine(30, 128, display.width() - 60, EPD_BLACK);

  // 4-level grayscale swatches
  int barW = (display.width() - 60) / 4;
  int barY = 148;
  int barH = 75;
  int startX = 30;

  display.fillRect(startX + 0 * barW, barY, barW, barH, EPD_BLACK);
  display.fillRect(startX + 1 * barW, barY, barW, barH, EPD_DARK);
  display.fillRect(startX + 2 * barW, barY, barW, barH, EPD_LIGHT);
  display.fillRect(startX + 3 * barW, barY, barW, barH, EPD_WHITE);
  display.drawRect(startX + 3 * barW, barY, barW, barH, EPD_BLACK);

  display.setTextSize(1);
  display.setTextColor(EPD_WHITE);
  display.setCursor(startX + 0 * barW + (barW - 30) / 2, barY + 32);
  display.print("Black");

  display.setCursor(startX + 1 * barW + (barW - 24) / 2, barY + 32);
  display.print("Dark");

  display.setTextColor(EPD_BLACK);
  display.setCursor(startX + 2 * barW + (barW - 30) / 2, barY + 32);
  display.print("Light");

  display.setCursor(startX + 3 * barW + (barW - 30) / 2, barY + 32);
  display.print("White");

  // Footer notes
  display.setTextSize(1);
  display.setTextColor(EPD_DARK);
  display.setCursor(30, 245);
  display.print("Native 2-bit grayscale LUT waveform on SSD1683.");
  display.setCursor(30, 265);
  display.print("Active HIGH BUSY pin, 24-pin FPC interface.");

  display.display();

  delay(10000);

  // Screen 2: Concentric geometry & test patterns
  Serial.println(F("Screen 2: Geometric grayscale test patterns"));
  display.clearBuffer();
  display.drawRect(0, 0, display.width(), display.height(), EPD_BLACK);

  // Concentric rounded rectangles with alternating gray shades
  int pad = 12;
  display.fillRoundRect(pad, pad, display.width() - 2 * pad, display.height() - 2 * pad, 12, EPD_LIGHT);
  pad += 16;
  display.fillRoundRect(pad, pad, display.width() - 2 * pad, display.height() - 2 * pad, 10, EPD_DARK);
  pad += 16;
  display.fillRoundRect(pad, pad, display.width() - 2 * pad, display.height() - 2 * pad, 8, EPD_BLACK);
  pad += 16;
  display.fillRoundRect(pad, pad, display.width() - 2 * pad, display.height() - 2 * pad, 6, EPD_WHITE);

  display.setTextColor(EPD_BLACK);
  display.setTextSize(3);
  printCentered("4-Level Gray", (display.height() - 24) / 2 - 15);

  display.setTextSize(2);
  display.setTextColor(EPD_DARK);
  printCentered("400 x 300 Pixels", (display.height() - 24) / 2 + 20);

  display.display();

  Serial.println(F("Done - display left on geometric pattern."));
}

void loop() {}

