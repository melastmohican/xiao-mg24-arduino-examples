/*****************************************************************************
* | File        :   XIAO_GDEY0266T90H.ino
* | Function    :   Drive Good Display GDEY0266T90H with Adafruit_EPD
* | Info        :
*----------------
* Drives the Good Display GDEY0266T90H (360x184, black/white) via Adafruit_EPD.
*
* The SSD1685 controller is register-compatible with the SSD1680 that Adafruit_EPD
* supports as Adafruit_SSD1680 (same 0x24/0x26 RAM, 0x22/0x20 update sequence,
* active-HIGH BUSY). This sketch defines a panel class locally
* (ThinkInk_266_Mono_GDEY0266T90H) that passes the 360x184 geometry - long axis
* first, as the 2.66" SSD1680 classes do - plus a mono init table taken from
* Good Display's sample, so stock Adafruit_EPD stays unmodified.
*
* Board       :   SiliconLabs:silabs:xiao_mg24:protocol_stack=none
* Panel       :   Good Display GDEY0266T90H, 360x184, SSD1685, FPC-H011
* Connection  :   Seeed ePaper Driver Board for XIAO v2, 24-pin FPC connector
*                 https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
*
* Pinout:
*   RST  -> D0
*   CS   -> D1
*   BUSY -> D2  (Active HIGH on SSD1685: 1 = busy, 0 = ready/idle)
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

// Rotation 2, applied after begin(): the 2.66" panels in this repo have their native
// origin at the corner opposite the one content should start from. Assumed the same
// here, not yet checked on hardware. Use 0 if the image comes out upside down.
#define DEMO_ROTATION 2

// Mono init, expressed as an Adafruit_EPD command list: {command, arg_count, args...},
// {0xFF, delay_ms}, terminated by 0xFE. RAM windows and driver output control are
// written by Adafruit_SSD1680::powerUp() after this list.
static const uint8_t gdey0266t90h_init_code[] = {
    SSD1680_SW_RESET, 0,                 // soft reset
    0xFF, 20,                            // busy wait
    SSD1680_DATA_MODE, 1, 0x03,          // RAM data entry mode
    SSD1680_WRITE_BORDER, 1, 0x05,       // border waveform
    SSD1680_DISP_CTRL1, 2, 0x40, 0x40,   // bypass RED RAM as 0; B[7:6]=01: 184-source panel (POR is 200)
    SSD1680_TEMP_CONTROL, 1, 0x80,       // built-in temperature sensor
    0xFE
};

class ThinkInk_266_Mono_GDEY0266T90H : public Adafruit_SSD1680 {
 public:
  ThinkInk_266_Mono_GDEY0266T90H(int16_t DC, int16_t RST, int16_t CS,
                                 int16_t SRCS, int16_t BUSY = -1,
                                 SPIClass *spi = &SPI)
      : Adafruit_SSD1680(360, 184, DC, RST, CS, SRCS, BUSY, spi) {}

  void begin(thinkinkmode_t mode = THINKINK_MONO) {
    Adafruit_SSD1680::begin(true);

    inkmode = mode;
    _xram_offset = 0;    // RAM column 0 is the first source on this panel
    _epd_init_code = gdey0266t90h_init_code;
    setColorBuffer(0, true);  // layer 0 uninverted
    setBlackBuffer(0, true);  // only one buffer

    layer_colors[EPD_WHITE] = 0b00;
    layer_colors[EPD_BLACK] = 0b01;
    layer_colors[EPD_RED] = 0b01;
    layer_colors[EPD_GRAY] = 0b01;
    layer_colors[EPD_LIGHT] = 0b00;
    layer_colors[EPD_DARK] = 0b01;

    _display_update_val = 0xF4;  // full refresh, ~2-3s
    default_refresh_delay = 3000;

    setRotation(DEMO_ROTATION);
    powerDown();
  }
};

ThinkInk_266_Mono_GDEY0266T90H display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
                                       EPD_BUSY, EPD_SPI);

void setup()
{
  Serial.begin(115200);
  // Serial is EUSART0 via the on-board CMSIS-DAP chip, not USB CDC, so bound
  // the wait - it must never block.
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  Serial.println();
  Serial.println(F("=================================================="));
  Serial.println(F(" Adafruit_EPD: GDEY0266T90H on XIAO MG24          "));
  Serial.println(F("=================================================="));

  display.begin();
  Serial.printf("Panel reports %d x %d\r\n", display.width(), display.height());

  Serial.println(F("Drawing test graphics in buffer..."));
  display.clearBuffer();

  const int W = display.width();    // 360 in landscape
  const int H = display.height();   // 184 in landscape

  // Header banner
  display.fillRect(0, 0, W, 28, EPD_BLACK);
  display.setTextColor(EPD_WHITE);
  display.setTextSize(2);
  display.setCursor(12, 7);
  display.print("2.66\" 360x184 B/W");

  // Typography
  display.setTextColor(EPD_BLACK);
  display.setTextSize(2);
  display.setCursor(12, 40);
  display.print("GDEY0266T90H");
  display.setTextSize(1);
  display.setCursor(12, 62);
  display.print("SSD1685 via Adafruit_SSD1680 + in-sketch panel class");
  display.drawFastHLine(12, 76, W - 24, EPD_BLACK);

  // Inverted label
  display.fillRoundRect(12, 86, 120, 22, 4, EPD_BLACK);
  display.setTextColor(EPD_WHITE);
  display.setCursor(24, 93);
  display.print("White on Black");
  display.drawRoundRect(144, 86, 120, 22, 4, EPD_BLACK);
  display.setTextColor(EPD_BLACK);
  display.setCursor(156, 93);
  display.print("Black on White");

  // Geometric shapes
  display.drawRect(12, 120, 60, 44, EPD_BLACK);
  display.fillRect(20, 128, 44, 28, EPD_BLACK);
  display.fillRect(30, 138, 24, 8, EPD_WHITE);

  display.drawCircle(120, 142, 22, EPD_BLACK);
  display.fillCircle(120, 142, 16, EPD_BLACK);
  display.fillCircle(120, 142, 8, EPD_WHITE);

  display.fillTriangle(170, 164, 200, 120, 230, 164, EPD_BLACK);
  display.drawTriangle(242, 164, 272, 120, 302, 164, EPD_BLACK);

  // Footer
  display.drawRect(0, 0, W, H, EPD_BLACK);

  Serial.println(F("Sending image buffer to display (refresh ~3s)..."));
  display.display();

  Serial.println(F("Powering down display..."));
  display.powerDown();

  Serial.println(F("Demo completed successfully."));
}

void loop()
{
  // Halt execution
}
