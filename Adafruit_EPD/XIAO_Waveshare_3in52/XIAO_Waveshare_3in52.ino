/*****************************************************************************
* | File        :   XIAO_Waveshare_3in52.ino
* | Function    :   Drive the Waveshare 3.52inch e-Paper (B) with Adafruit_EPD
* | Info        :
*----------------
* The Waveshare 3.52" (B) panel (240x360 BWR) uses a UC8253 controller, the same
* one Adafruit_EPD already drives for its 3.7" tricolor panel
* (ThinkInk_370_Tricolor_BABMFGNR, Adafruit product 6395). Only the resolution
* and init values differ, so this sketch defines a panel class locally rather
* than patching the library - it keeps working across Adafruit_EPD updates.
*
* Init values are lifted from the known-good Waveshare driver in the
* Waveshare_3in52 sketch in this repo, which is verified working on this panel.
*
* Board : SiliconLabs:silabs:xiao_mg24:protocol_stack=none
* Panel : Waveshare 3.52inch e-Paper (B), 240x360, UC8253, seated in the Seeed
*         Studio ePaper Driver Board for XIAO v2's 24-pin FPC connector
*         https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
*
* Based on Adafruit's ThinkInk_tricolor example.
******************************************************************************/
#include "Adafruit_ThinkInk.h"

// Pin mapping is fixed by the Seeed ePaper Driver Board v2, the same one the
// GxEPD2 demos in this repo use.
#define EPD_RESET D0
#define EPD_CS D1
#define EPD_BUSY D2
#define EPD_DC D3
#define SRAM_CS -1    // no SRAM chip on the driver board - use onboard RAM
// Default SPI: D8/D10 are already the MG24's hardware SCK/MOSI, so unlike the
// Feather ThinkInk (which puts the EPD on SPI1) there is no secondary bus here.
#define EPD_SPI &SPI

// Waveshare's init sequence, expressed as an Adafruit_EPD command list:
//   {command, arg_count, args...}, with {0xFF, ms} meaning busy_wait + delay,
//   terminated by 0xFE.
// Unlike Adafruit's 3.7" panel, which encodes its size in the PSR bits, this
// panel needs an explicit resolution (TRES, 0x61).
#define UC8253_RESOLUTION 0x61
#define UC8253_BOOSTER 0x06

static const uint8_t ws_3in52b_init_code[]{
    UC8253_POWERON,     0,                      // 0x04
    0xFF,               100,                    // wait for BUSY, then 100ms
    UC8253_VCOM_CDI,    1, 0x87,                // 0x50
    UC8253_PANELSETTING, 2, 0x03, 0x0D,         // 0x00
    UC8253_RESOLUTION,  3, 0xF0, 0x01, 0x68,    // 0x61: 240 x 0x0168 (360)
    UC8253_BOOSTER,     3, 0x2F, 0x2F, 0x2E,    // 0x06
    0xFE};

class ThinkInk_352_Tricolor_Waveshare : public Adafruit_UC8253 {
 public:
  ThinkInk_352_Tricolor_Waveshare(int16_t DC, int16_t RST, int16_t CS,
                                  int16_t SRCS, int16_t BUSY = -1,
                                  SPIClass *spi = &SPI)
      // Adafruit_UC8253 pads *height* to a multiple of 8 and sizes the buffer as
      // width*height/8, so its "height" is the controller's HRES (240 here) and
      // its "width" is the VRES / line count (360). Long axis first, matching
      // ThinkInk_370_Tricolor_BABMFGNR's (416, 240) for a 240x416 raster.
      // Passing these the other way round keeps the buffer the right total size
      // but strides it 45 bytes per line instead of 30, which shears and repeats
      // the image.
      : Adafruit_UC8253(360, 240, DC, RST, CS, SRCS, BUSY, spi){};

  void begin(thinkinkmode_t mode = THINKINK_TRICOLOR) {
    Adafruit_UC8253::begin(true);

    // Plane order from Adafruit_UC8253::begin() is right for this panel:
    // black -> buffer 0 (RAM 0x10), red -> buffer 1 (RAM 0x13), matching the
    // Waveshare driver. The black plane's polarity is not: this panel wants it
    // uninverted, where Adafruit's default (and their 3.7" panel) inverts it.
    // Leaving it inverted renders white as black and black as white, with red
    // unaffected. Red's default polarity is correct as-is.
    setBlackBuffer(0, false);

    inkmode = mode;
    _epd_init_code = ws_3in52b_init_code;

    // layer_colors is deliberately left at the base class defaults (BLACK 0b01,
    // RED 0b10). The bit position indexes the buffer, so it has to agree with
    // the buffer assignment above - overriding one without the other routes
    // black pixels into the red plane.

    // Only a fallback for BUSY == -1; a full refresh on this panel is ~20s.
    default_refresh_delay = 25000;
    setRotation(0);
    // DO NOT power down again!
  }
};

ThinkInk_352_Tricolor_Waveshare display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
                                        EPD_BUSY, EPD_SPI);

void setup() {
  Serial.begin(115200);
  // Serial here is EUSART0 bridged through the on-board CMSIS-DAP chip, not a
  // USB CDC endpoint, so bound the wait - unbounded, it can hang the sketch.
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  Serial.println("Waveshare 3.52in (B) on Adafruit_EPD / UC8253");
  display.begin(THINKINK_TRICOLOR);
  Serial.printf("Panel reports %d x %d\r\n", display.width(), display.height());

  // Single-shot, not a loop: the panel's datasheet suggests updating about once
  // a day, so two refreshes and stop rather than cycling every 20 seconds.
  Serial.println("Banner demo");
  display.clearBuffer();
  display.setTextSize(3);
  display.setCursor((display.width() - 144) / 2, (display.height() - 24) / 2);
  display.setTextColor(EPD_BLACK);
  display.print("Tri");
  display.setTextColor(EPD_RED);
  display.print("Color");
  display.display();

  delay(20000);

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
