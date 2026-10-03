/*****************************************************************************
* | File        :   XIAO_GDEW0215T12.ino
* | Function    :   Drive the Good Display GDEW0215T12 with Adafruit_EPD
* | Info        :
*----------------
* The Good Display GDEW0215T12 panel (formerly GDEW0215T11, 208x112 monochrome)
* uses a UC8151D controller. Adafruit_EPD includes an Adafruit_UC8151D driver,
* which we specialize here with an in-sketch panel class
* (ThinkInk_215_Mono_GDEW0215T12) specifying the exact 208x112 resolution, panel
* settings, and partial refresh LUT.
*
* Board       :   SiliconLabs:silabs:xiao_mg24:protocol_stack=none
* Panel       :   GDEW0215T12 (formerly GDEW0215T11), 208x112 monochrome, UC8151D, 24-pin FPC
* Driver Board:   Seeed Studio ePaper Driver Board for XIAO v2
*                 https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
*
* Pinout:
*   RST  -> D0
*   CS   -> D1
*   BUSY -> D2  (Active LOW on UC8151D: 0 = busy, 1 = ready)
*   DC   -> D3
*   SCK  -> D8
*   MOSI -> D10
******************************************************************************/
#include "Adafruit_ThinkInk.h"

// Seeed ePaper Driver Board for XIAO v2 wiring
#define EPD_RESET D0
#define EPD_CS    D1
#define EPD_BUSY  D2
#define EPD_DC    D3
#define SRAM_CS   -1   // No external SRAM on this shield, use MCU RAM
#define EPD_SPI   &SPI // D8=SCK / D10=MOSI are the default hardware SPI

// Custom init code for 208x112 GDEW0215T12 panel
static const uint8_t gdew0215t12_monofull_init_code[]{
    UC8151D_PSR, 2, 0x1F, 0x0D,               // panel setting (OTP LUT)
    0x61,        3, 112, 0, 208,              // resolution setting: 112 x 208
    UC8151D_PON, 0,                           // power on
    0xFF,        10,                          // wait busy + 10ms
    UC8151D_CDI, 1, 0x97,                     // VCOM and data interval
    0xFE};

static const uint8_t gdew0215t12_partial_init_code[]{
    UC8151D_PWR,  5, 0x03, 0x00, 0x2B, 0x2B, 0x03, // power setting
    UC8151D_BTST, 3, 0x17, 0x17, 0x17,             // boost soft start
    UC8151D_PSR,  2, 0xBF, 0x0D,                   // panel setting (registers LUT)
    UC8151D_PLL,  1, 0x3C,                         // PLL frame rate
    0x61,         3, 112, 0, 208,                  // resolution setting: 112 x 208
    UC8151D_VDCS, 1, 0x12,                         // vcom_DC setting
    UC8151D_PON,  0,                               // power on
    0xFF,         10,                              // wait busy + 10ms
    UC8151D_CDI,  1, 0x97,                         // CDI setting
    0xFE};

class ThinkInk_215_Mono_GDEW0215T12 : public Adafruit_UC8151D {
 public:
  ThinkInk_215_Mono_GDEW0215T12(int16_t DC, int16_t RST, int16_t CS,
                                int16_t SRCS, int16_t BUSY = -1,
                                SPIClass *spi = &SPI)
      // Long axis first: 208 x 112
      : Adafruit_UC8151D(208, 112, DC, RST, CS, SRCS, BUSY, spi){};

  void begin(thinkinkmode_t mode = THINKINK_MONO) {
    Adafruit_UC8151D::begin(true);
    setColorBuffer(1, true);
    setBlackBuffer(1, true);

    // Initialize both buffers to 0xFF (white) so the differential LUT
    // does not compare against uninitialized heap memory.
    if (buffer1) {
      memset(buffer1, 0xFF, buffer1_size);
    }
    if (buffer2) {
      memset(buffer2, 0xFF, buffer2_size);
    }

    inkmode = mode;
    _epd_init_code = gdew0215t12_monofull_init_code;
    _epd_partial_init_code = gdew0215t12_partial_init_code;
    _epd_partial_lut_code = uc8151d_partialmono_lut;

    layer_colors[EPD_WHITE] = 0b00;
    layer_colors[EPD_BLACK] = 0b01;
    layer_colors[EPD_RED]   = 0b01;
    layer_colors[EPD_GRAY]  = 0b01;
    layer_colors[EPD_LIGHT] = 0b00;
    layer_colors[EPD_DARK]  = 0b01;

    default_refresh_delay = 3000;
    // Rotation 0 is native landscape (208 x 112) in Adafruit_UC8151D.
    // Rotation 1 or 3 would swap width/height into 112x208 portrait.
    setRotation(0);
    powerDown();
  }

  void display(bool sleep = false) {
    // For full refresh with UC8151D OTP LUT, buffer 0 (0x10) must be 0xFF (all white)
    // matching Good Display reference implementation.
    if (buffer1) {
      memset(buffer1, 0xFF, buffer1_size);
    }
    Adafruit_UC8151D::display(sleep);
    // Sync buffer1 with buffer2 so subsequent partial updates have valid previous-frame data
    if (buffer1 && buffer2) {
      memcpy(buffer1, buffer2, buffer1_size);
    }
  }

  void displayPartial(uint16_t x1 = 0, uint16_t y1 = 0, uint16_t x2 = 0, uint16_t y2 = 0) {
    (void)x1; (void)y1; (void)x2; (void)y2;

    const uint8_t *init_code_backup = _epd_init_code;
    const uint8_t *lut_code_backup = _epd_lut_code;
    _epd_init_code = gdew0215t12_partial_init_code;
    _epd_partial_lut_code = uc8151d_partialmono_lut;
    _epd_lut_code = uc8151d_partialmono_lut;

    // Power up and load partial LUT on first partial update after full refresh
    if (partialsSinceLastFullUpdate == 0) {
      powerUp();
    }

    // Enter partial mode
    EPD_command(UC8151D_PTIN); // 0x91

    // Set partial window to the full 112x208 resolution (14 bytes x 208 gates = 2912 bytes)
    // to match full-framebuffer transfer without offset mismatch.
    uint8_t buf[7];
    buf[0] = 0;           // x_start (source column 0)
    buf[1] = 111;         // x_end (source column 111, 112 channels)
    buf[2] = 0;           // y_start high
    buf[3] = 0;           // y_start low (gate 0)
    buf[4] = 0;           // y_end high
    buf[5] = 207;         // y_end low (gate 207, 208 gates)
    buf[6] = 0x28;        // PT_SCAN
    EPD_command(UC8151D_PTL, buf, 7); // 0x90

    // Transfer buffer1 (previous frame) to 0x10 and buffer2 (new frame) to 0x13
    // without inversion (invertdata = false).
    writeRAMFramebufferToEPD(buffer1, buffer1_size, 0, false);
    delay(2);
    writeRAMFramebufferToEPD(buffer2, buffer2_size, 1, false);

    // Refresh with the 0.5s partial LUT
    update();

    // Exit partial mode
    EPD_command(UC8151D_PTOUT); // 0x92

    // Update buffer1 to match buffer2 for subsequent differential updates
    memcpy(buffer1, buffer2, buffer1_size);
    partialsSinceLastFullUpdate++;

    _epd_lut_code = lut_code_backup;
    _epd_init_code = init_code_backup;
  }
};

ThinkInk_215_Mono_GDEW0215T12 display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
                                      EPD_BUSY, EPD_SPI);

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  Serial.println(F("GDEW0215T12 2.15in Mono on Adafruit_EPD / UC8151D"));

  display.begin(THINKINK_MONO);
  Serial.printf("Panel reports %d x %d\r\n", display.width(), display.height());

  // Screen 1: Typography & Banner
  Serial.println(F("Screen 1: Banner & Typography"));
  display.clearBuffer();
  display.drawRect(0, 0, display.width(), display.height(), EPD_BLACK);
  display.drawRect(2, 2, display.width() - 4, display.height() - 4, EPD_BLACK);

  display.setTextSize(2);
  display.setTextColor(EPD_BLACK);
  display.setCursor(16, 12);
  display.print("GDEW0215T12");

  display.setTextSize(1);
  display.setCursor(16, 38);
  display.print("2.15\" 208x112 Mono");
  display.setCursor(16, 52);
  display.print("Driver: UC8151D");
  display.setCursor(16, 66);
  display.print("XIAO MG24 Sense");

  // Bottom info strip
  display.fillRect(4, 88, display.width() - 8, 20, EPD_BLACK);
  display.setTextColor(EPD_WHITE);
  display.setCursor(14, 94);
  display.print("Adafruit_EPD Driver");

  display.display();
  delay(5000);

  // Screen 2: Geometry & Patterns
  Serial.println(F("Screen 2: Geometry & Test Patterns"));
  display.clearBuffer();
  display.drawRect(0, 0, display.width(), display.height(), EPD_BLACK);

  // Circles
  display.drawCircle(30, 36, 22, EPD_BLACK);
  display.fillCircle(30, 36, 14, EPD_BLACK);

  // Rounded rectangles
  display.drawRoundRect(65, 14, 50, 44, 6, EPD_BLACK);
  display.fillRoundRect(70, 19, 40, 34, 4, EPD_BLACK);

  // Checkered pattern box across x=128..195
  for (int y = 14; y < 58; y += 4) {
    for (int x = 130; x < 194; x += 4) {
      if (((x / 4) + (y / 4)) % 2 == 0) {
        display.fillRect(x, y, 4, 4, EPD_BLACK);
      }
    }
  }
  display.drawRect(128, 13, 67, 47, EPD_BLACK);

  // Status text
  display.setTextSize(1);
  display.setTextColor(EPD_BLACK);
  display.setCursor(12, 75);
  display.print("Full Refresh: 3.0s");
  display.setCursor(12, 90);
  display.print("Partial Refresh: 0.5s");

  display.display();
  delay(5000);

  // Screen 3: Partial Refresh Demonstration
  Serial.println(F("Screen 3: Partial Refresh Counter (0..5)"));
  display.clearBuffer();
  display.drawRect(0, 0, display.width(), display.height(), EPD_BLACK);
  display.setTextSize(1);
  display.setTextColor(EPD_BLACK);
  display.setCursor(16, 14);
  display.print("Partial Refresh Test:");
  display.drawRect(70, 32, 68, 56, EPD_BLACK);
  display.display(); // Base frame update

  for (int count = 0; count <= 5; count++) {
    display.fillRect(72, 34, 64, 52, EPD_WHITE);
    display.setTextSize(3);
    display.setTextColor(EPD_BLACK);
    display.setCursor(94, 46);
    display.print(count);

    Serial.printf("Partial update: count = %d\r\n", count);
    // Partial update bounding box: x in [70, 138], y in [32, 88] (y multiple of 8)
    display.displayPartial(70, 32, 138, 88);
    delay(500);
  }

  Serial.println(F("Putting display into deep sleep..."));
  display.powerDown();
  Serial.println(F("Demo completed."));
}

void loop() {}
