/*****************************************************************************
* | File        :   XIAO_GDEM0154F61H.ino
* | Function    :   Drive Good Display GDEM0154F61H with Adafruit_EPD
* | Info        :
*----------------
* Drives the Good Display GDEM0154F61H (200x200, 4-color: Black, White,
* Yellow, Red) via Adafruit_EPD.
*
* The SSD2681 controller shares its 2-bit RAM layout and command structure
* (0x10 RAM start, 0x12 refresh, 0x02 power off, 0x07 sleep, active-low BUSY)
* with JD79661, which Adafruit_EPD supports via Adafruit_JD79661. This sketch
* defines a panel class locally (ThinkInk_154_Quadcolor_GDEM0154F61H) with the
* 200x200 resolution and SSD2681 initialization table so stock Adafruit_EPD
* remains unmodified.
*
* Board       :   SiliconLabs:silabs:xiao_mg24:protocol_stack=none
* Panel       :   Good Display GDEM0154F61H, 200x200, SSD2681 controller, FPC-8101
* Connection  :   Seeed ePaper Driver Board for XIAO v2, 24-pin FPC connector
*                 https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
*
* Pinout:
*   RST  -> D0
*   CS   -> D1
*   BUSY -> D2  (Active LOW on SSD2681: 0 = busy, 1 = ready/idle)
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

// SSD2681 initialization sequence expressed as an Adafruit_EPD command list:
// {command, arg_count, args...}, {0xFF, delay_ms}, terminated by 0xFE.
static const uint8_t gdem0154f61h_init_code[] = {
    0xFF, 10,
    0xE9, 1, 0x01,
    0x04, 0,                                     // POWER_ON
    0xFE
};

class ThinkInk_154_Quadcolor_GDEM0154F61H : public Adafruit_JD79661 {
 public:
  ThinkInk_154_Quadcolor_GDEM0154F61H(int16_t DC, int16_t RST, int16_t CS,
                                      int16_t SRCS, int16_t BUSY = -1,
                                      SPIClass *spi = &SPI)
      : Adafruit_JD79661(200, 200, DC, RST, CS, SRCS, BUSY, spi) {}

  void begin(thinkinkmode_t mode = THINKINK_QUADCOLOR) {
    Adafruit_JD79661::begin(true);

    inkmode = mode;
    _epd_init_code = gdem0154f61h_init_code;
    default_refresh_delay = 20000;
    setRotation(0);
    powerDown();
  }
};

ThinkInk_154_Quadcolor_GDEM0154F61H display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
                                           EPD_BUSY, EPD_SPI);

void setup()
{
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  Serial.println();
  Serial.println(F("=================================================="));
  Serial.println(F(" Adafruit_EPD: GDEM0154F61H on XIAO MG24          "));
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
  display.print("Adafruit_EPD + SSD2681");

  Serial.println(F("Sending image buffer to display (refresh ~20s)..."));
  display.display();

  Serial.println(F("Powering down display..."));
  display.powerDown();

  Serial.println(F("Demo completed successfully."));
}

void loop()
{
  // Halt execution
}
