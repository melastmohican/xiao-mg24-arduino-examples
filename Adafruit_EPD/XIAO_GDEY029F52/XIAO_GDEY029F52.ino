/*****************************************************************************
* | File        :   XIAO_GDEY029F52.ino
* | Function    :   Drive Good Display GDEY029F52 with Adafruit_EPD
* | Info        :
*----------------
* Drives the Good Display GDEY029F52 (296x128, 4-color: Black, White, Yellow,
* Red) via Adafruit_EPD.
*
* The panel uses a JD79661 controller, which Adafruit_EPD supports through
* Adafruit_JD79661, but Adafruit ships no 2.9" quad-color ThinkInk panel class.
* This sketch defines one locally (ThinkInk_290_Quadcolor_GDEY029F52) with the
* init table from Good Display's own AU-GDEY029F52-F-20250609 sample, and leaves
* stock Adafruit_EPD unmodified. The sample's PSR value (0x0F 0x29) differs from
* Adafruit's default JD79661 init (0x8F 0x29), so the default table is not used.
*
* The native raster is 128 x 296 (a byte multiple, so no padding is needed).
* Rotation 1 gives the 296 x 128 landscape view used below.
*
* Board       :   SiliconLabs:silabs:xiao_mg24:protocol_stack=none
* Panel       :   Good Display GDEY029F52, 296x128, JD79661 controller
* Connection  :   Seeed ePaper Driver Board for XIAO v2, 24-pin FPC connector
*                 https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
*
* Pinout:
*   RST  -> D0
*   CS   -> D1
*   BUSY -> D2  (Active LOW on JD79661: 0 = busy, 1 = ready/idle)
*   DC   -> D3
*   SCK  -> D8
*   MOSI -> D10
******************************************************************************/
#include "Adafruit_ThinkInk.h"

// Pin mapping fixed by Seeed ePaper Driver Board v2
#define EPD_RESET   D0
#define EPD_CS      D1
#define EPD_BUSY    D2
#define EPD_DC      D3
#define SRAM_CS     -1    // No external SRAM chip, use MCU RAM
#define EPD_SPI     &SPI

// Native raster is 128x296; rotation 1 = 296x128 landscape, 3 = same rotated 180 degrees.
#define DEMO_ROTATION 1

// JD79661 init sequence from Good Display's GDEY029F52 sample, expressed as an
// Adafruit_EPD command list: {command, arg_count, args...}, {0xFF, delay_ms},
// terminated by 0xFE.
static const uint8_t gdey029f52_init_code[] = {
    0xFF, 10,
    0x4D, 1, 0x78,
    0x00, 2, 0x0F, 0x29,                         // PSR
    0x01, 2, 0x07, 0x00,                         // PWRR
    0x03, 3, 0x10, 0x54, 0x44,                   // POFS
    0x06, 7, 0x05, 0x00, 0x3F, 0x0A, 0x25, 0x12, 0x1A, // BTST_P
    0x50, 1, 0x37,                               // CDI
    0x60, 2, 0x02, 0x02,                         // TCON
    0x61, 4, 0x00, 0x80, 0x01, 0x28,             // TRES: 128 x 296
    0xE7, 1, 0x1C,
    0xE3, 1, 0x22,
    0xB4, 1, 0xD0,
    0xB5, 1, 0x03,
    0xE9, 1, 0x01,
    0x30, 1, 0x08,                               // PLL
    0x04, 0,                                     // POWER_ON
    0xFE
};

class ThinkInk_290_Quadcolor_GDEY029F52 : public Adafruit_JD79661 {
 public:
  ThinkInk_290_Quadcolor_GDEY029F52(int16_t DC, int16_t RST, int16_t CS,
                                    int16_t SRCS, int16_t BUSY = -1,
                                    SPIClass *spi = &SPI)
      : Adafruit_JD79661(128, 296, DC, RST, CS, SRCS, BUSY, spi) {}

  void begin(thinkinkmode_t mode = THINKINK_QUADCOLOR) {
    Adafruit_JD79661::begin(true);

    inkmode = mode;
    _epd_init_code = gdey029f52_init_code;
    default_refresh_delay = 16000;
    setRotation(DEMO_ROTATION);
    powerDown();
  }
};

ThinkInk_290_Quadcolor_GDEY029F52 display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
                                          EPD_BUSY, EPD_SPI);

// Print text horizontally centered on the display at the given top-left y,
// using the current text size (measured, not hard-coded).
void printCentered(const char* text, int16_t y)
{
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, y, &x1, &y1, &w, &h);
  display.setCursor((display.width() - w) / 2 - x1, y);
  display.print(text);
}

void setup()
{
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  Serial.println();
  Serial.println(F("=================================================="));
  Serial.println(F(" Adafruit_EPD: GDEY029F52 on XIAO MG24            "));
  Serial.println(F("=================================================="));

  display.begin();

  Serial.print(F("Panel reports "));
  Serial.print(display.width());
  Serial.print(F(" x "));
  Serial.println(display.height());

  Serial.println(F("Drawing test graphics in buffer..."));
  display.clearBuffer();

  const int W = display.width();    // 296 in landscape
  const int H = display.height();   // 128 in landscape

  // Top header banner
  display.fillRect(0, 0, W, 24, EPD_RED);
  display.setTextColor(EPD_WHITE);
  display.setTextSize(2);
  printCentered("2.9\" 4-COLOR", 5);

  // Quad-color text lines
  display.setTextSize(1);
  display.setTextColor(EPD_BLACK);
  display.setCursor(10, 32);
  display.print("BLACK: Primary Text");

  display.setTextColor(EPD_RED);
  display.setCursor(10, 44);
  display.print("RED:   Alerts");

  display.setTextColor(EPD_YELLOW);
  display.setCursor(10, 56);
  display.print("YELLOW: Warnings");

  display.drawFastHLine(10, 68, W - 20, EPD_BLACK);

  // 4 color swatches
  const uint16_t swColors[] = {EPD_BLACK, EPD_WHITE, EPD_RED, EPD_YELLOW};
  const char* swNames[] = {"BLK", "WHT", "RED", "YEL"};
  const int sw = 44, sh = 28, gap = 6;
  const int sx = 10;
  const int sy = 76;

  for (int i = 0; i < 4; i++) {
    int x = sx + i * (sw + gap);
    display.fillRoundRect(x, sy, sw, sh, 3, swColors[i]);
    display.drawRoundRect(x, sy, sw, sh, 3, EPD_BLACK);
    display.setTextColor(swColors[i] == EPD_BLACK || swColors[i] == EPD_RED ? EPD_WHITE : EPD_BLACK);
    int16_t x1, y1; uint16_t tw, th;
    display.getTextBounds(swNames[i], 0, 0, &x1, &y1, &tw, &th);
    display.setCursor(x + (sw - tw) / 2 - x1, sy + (sh - th) / 2 - y1);
    display.print(swNames[i]);
  }

  // Concentric geometric shapes, right of the text lines
  display.drawCircle(160, 50, 18, EPD_BLACK);
  display.fillCircle(160, 50, 14, EPD_RED);
  display.fillCircle(160, 50, 8, EPD_YELLOW);

  display.drawRect(200, 32, 56, 36, EPD_RED);
  display.fillRect(205, 37, 46, 26, EPD_YELLOW);
  display.fillRect(213, 44, 30, 12, EPD_BLACK);

  display.fillTriangle(W - 70, 108, W - 45, 78, W - 20, 108, EPD_RED);
  display.drawTriangle(W - 70, 108, W - 45, 78, W - 20, 108, EPD_BLACK);

  // Footer status bar
  display.fillRect(0, H - 14, W - 100, 14, EPD_BLACK);
  display.setTextColor(EPD_YELLOW);
  display.setCursor(6, H - 11);
  display.print("Adafruit_EPD + JD79661");

  Serial.println(F("Sending image buffer to display (refresh ~16s)..."));
  display.display();

  Serial.println(F("Powering down display..."));
  display.powerDown();

  Serial.println(F("Demo completed successfully."));
}

void loop()
{
  // Halt execution
}
