/*****************************************************************************
* | File        :   XIAO_Waveshare_1in54g.ino
* | Function    :   Drive Waveshare 1.54" e-Paper (G) with Adafruit_EPD
* | Info        :
*----------------
* Drives the Waveshare 1.54inch e-Paper (G) / Good Display GDEM0154F51H
* (200x200, 4-color: Black, White, Yellow, Red) via Adafruit_EPD.
*
* The JD79660 controller shares its register and RAM layout with JD79661,
* which Adafruit_EPD supports via Adafruit_JD79661. This sketch defines a panel
* class locally (ThinkInk_154_Quadcolor_Waveshare) with the 200x200 resolution
* and JD79660 initialization table so stock Adafruit_EPD remains unmodified.
*
* Board       :   SiliconLabs:silabs:xiao_mg24:protocol_stack=none
* Panel       :   Waveshare 1.54inch e-Paper (G), SKU 30441, FPC-8101, JD79660AA
* Connection  :   Seeed ePaper Driver Board for XIAO v2, 24-pin FPC connector
*                 https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
*
* Pinout:
*   RST  -> D0
*   CS   -> D1
*   BUSY -> D2  (Active LOW on JD79660)
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
#define SRAM_CS     -1    // No external SRAM chip - use MCU RAM
#define EPD_SPI     &SPI

// JD79660 initialization sequence expressed as an Adafruit_EPD command list:
// {command, arg_count, args...}, {0xFF, delay_ms}, terminated by 0xFE.
static const uint8_t ws_1in54g_init_code[] = {
    0xFF, 10,
    0x4D, 1, 0x78,
    0x00, 2, 0x0F, 0x29,                         // PSR: 0x0F, 0x29
    0x06, 7, 0x0D, 0x12, 0x30, 0x20, 0x19, 0x2A, 0x22, // BTST_P
    0x50, 1, 0x37,                               // CDI: 0x37
    0x61, 4, 0, 200, 0, 200,                     // TRES: 200 x 200
    0xE9, 1, 0x01,
    0x30, 1, 0x08,                               // PLL: 0x08
    0x04, 0,                                     // POWER_ON
    0xFE
};

class ThinkInk_154_Quadcolor_Waveshare : public Adafruit_JD79661 {
 public:
  ThinkInk_154_Quadcolor_Waveshare(int16_t DC, int16_t RST, int16_t CS,
                                   int16_t SRCS, int16_t BUSY = -1,
                                   SPIClass *spi = &SPI)
      : Adafruit_JD79661(200, 200, DC, RST, CS, SRCS, BUSY, spi) {}

  void begin(thinkinkmode_t mode = THINKINK_QUADCOLOR) {
    Adafruit_JD79661::begin(true);

    inkmode = mode;
    _epd_init_code = ws_1in54g_init_code;
    default_refresh_delay = 20000;
    setRotation(0);
    powerDown();
  }
};

ThinkInk_154_Quadcolor_Waveshare display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
                                        EPD_BUSY, EPD_SPI);

void setup()
{
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  Serial.println();
  Serial.println(F("=================================================="));
  Serial.println(F(" Adafruit_EPD: Waveshare 1.54\" (G) on XIAO MG24   "));
  Serial.println(F("=================================================="));

  display.begin();

  Serial.println(F("Drawing test graphics in buffer..."));
  display.clearBuffer();

  // Top header banner
  display.fillRect(0, 0, 200, 24, EPD_RED);
  display.setTextColor(EPD_WHITE);
  display.setTextSize(2);
  display.setCursor(10, 5);
  display.print("1.54\" 4-COLOR");

  // Quad-color text lines
  display.setTextSize(1);
  display.setTextColor(EPD_BLACK);
  display.setCursor(10, 32);
  display.print("BLACK: Primary Text");

  display.setTextColor(EPD_RED);
  display.setCursor(10, 46);
  display.print("RED:   Alerts & Accents");

  display.setTextColor(EPD_YELLOW);
  display.setCursor(10, 60);
  display.print("YELLOW: Warnings & Highs");

  display.drawFastHLine(10, 74, 180, EPD_BLACK);

  // 4 Color swatches
  const uint16_t swColors[] = {EPD_BLACK, EPD_WHITE, EPD_RED, EPD_YELLOW};
  const char* swNames[] = {"BLK", "WHT", "RED", "YEL"};
  int sw = 38, sh = 28, gap = 6;
  int sx = (200 - (4 * sw + 3 * gap)) / 2;
  int sy = 82;

  for (int i = 0; i < 4; i++) {
    int x = sx + i * (sw + gap);
    display.fillRoundRect(x, sy, sw, sh, 3, swColors[i]);
    display.drawRoundRect(x, sy, sw, sh, 3, EPD_BLACK);
    display.setTextColor(swColors[i] == EPD_BLACK || swColors[i] == EPD_RED ? EPD_WHITE : EPD_BLACK);
    display.setCursor(x + 10, sy + 10);
    display.print(swNames[i]);
  }

  // Concentric geometric shapes
  display.drawRect(10, 118, 50, 40, EPD_RED);
  display.fillRect(15, 123, 40, 30, EPD_YELLOW);
  display.fillRect(22, 130, 26, 16, EPD_BLACK);

  display.drawCircle(95, 138, 18, EPD_BLACK);
  display.fillCircle(95, 138, 14, EPD_RED);
  display.fillCircle(95, 138, 8, EPD_YELLOW);

  display.fillTriangle(145, 156, 165, 120, 185, 156, EPD_RED);
  display.drawTriangle(145, 156, 165, 120, 185, 156, EPD_BLACK);

  // Footer status bar
  display.fillRect(0, 178, 200, 22, EPD_BLACK);
  display.setTextColor(EPD_YELLOW);
  display.setCursor(16, 184);
  display.print("Adafruit_EPD + JD79660");

  Serial.println(F("Sending image buffer to display (refresh ~20s)..."));
  display.display();

  Serial.println(F("Powering down display..."));
  display.powerDown();

  Serial.println(F("Demo completed successfully."));
}

void loop()
{
  // Nothing to do in loop
}
