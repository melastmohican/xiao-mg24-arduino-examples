/*****************************************************************************
* | File        :   XIAO_GDEW0102I4FC.ino
* | Function    :   Drive Good Display GDEW0102I4FC with Adafruit_EPD on XIAO MG24
* | Info        :
*----------------
* Drives the Good Display GDEW0102I4FC (80x128 monochrome flexible e-paper)
* via Adafruit_EPD on Seeed Studio XIAO MG24 with DESPI-C102.
*
* The UC8175 controller shares its RAM layout and command protocol
* (0x00 PSR, 0x04 PON, 0x10 DTM1, 0x12 DRF, 0x13 DTM2, 0x02 POF, 0x07 DSLP,
* active-low BUSY) with UC8151D, which Adafruit_EPD supports via
* Adafruit_UC8151D. This sketch defines a panel class locally
* (ThinkInk_102_Mono_GDEW0102I4FC) with the 80x128 resolution and UC8175
* initialization table so stock Adafruit_EPD remains unmodified.
*
* Board       :   SiliconLabs:silabs:xiao_mg24:protocol_stack=none
* Panel       :   Good Display GDEW0102I4FC, 80x128, UC8175 controller, flexible
* Adapter     :   Good Display DESPI-C102_30P connector board
*
* Pinout (Direct 3.3V connection, no level shifter needed):
*   RST  -> D0
*   CS   -> D1
*   BUSY -> D2  (Active LOW on UC8175: 0 = busy, 1 = ready/idle)
*   DC   -> D3
*   SCK  -> D8
*   MOSI -> D10
*   GND  -> GND
*   VDD  -> 3V3
******************************************************************************/
#include "Adafruit_ThinkInk.h"
#include "drivers/Adafruit_UC8151D.h"

#define EPD_RESET   D0
#define EPD_CS      D1
#define EPD_BUSY    D2
#define EPD_DC      D3
#define SRAM_CS     -1    // No external SRAM chip, use MCU RAM
#define EPD_SPI     &SPI

// UC8175 initialization sequence expressed as an Adafruit_EPD command list:
// {command, arg_count, args...}, {0xFF, delay_ms}, terminated by 0xFE.
static const uint8_t gdew0102i4fc_init_code[] = {
  0x00, 1, 0x5F,       // Panel setting: LUT from OTP, 80x128
  0x2A, 2, 0x00, 0x00, // LUT option
  0x04, 0,             // Power on
  0xFF, 10,            // 10ms delay
  0x50, 1, 0x97,       // CDI (VCOM and data interval)
  0xFE                 // End of command list
};

class ThinkInk_102_Mono_GDEW0102I4FC : public Adafruit_UC8151D {
 public:
  ThinkInk_102_Mono_GDEW0102I4FC(int16_t DC, int16_t RST, int16_t CS,
                                 int16_t SRCS, int16_t BUSY = -1,
                                 SPIClass *spi = &SPI)
      // Long axis first: 128 x 80. Adafruit_EPD packs HEIGHT / 8 bytes per slice;
      // 80 / 8 = 10 bytes per scanline across 128 scanlines matches UC8175 hardware RAM.
      : Adafruit_UC8151D(128, 80, DC, RST, CS, SRCS, BUSY, spi) {}

  void busy_wait(void) override {
    if (_busy_pin >= 0) {
      uint32_t start = millis();
      // Active LOW on UC8175: 0 = busy, 1 = ready/idle.
      // Poll hardware pin directly without sending SPI commands during busy state.
      while (digitalRead(_busy_pin) == LOW) {
        if (millis() - start > 10000) {
          Serial.println(F("GDEW0102I4FC BUSY timeout!"));
          break;
        }
        delay(1);
      }
    } else {
      delay(500);
    }
  }

  void powerUp(void) override {
    hardwareReset();
    delay(10);
    busy_wait();

    const uint8_t *init_code = _epd_init_code ? _epd_init_code : gdew0102i4fc_init_code;
    EPD_commandList(init_code);
    busy_wait();
  }

  void begin(thinkinkmode_t mode = THINKINK_MONO) {
    Adafruit_UC8151D::begin(true);

    inkmode = mode;
    _epd_init_code = gdew0102i4fc_init_code;
    default_refresh_delay = 3000;
    // Rotation 1 maps native 128x80 into 80x128 portrait with row-major raster
    setRotation(1);
    powerDown();
  }

  void display(bool sleep = false) {
    // Ensure buffer1 (0x10 old frame) is clean white (0xFF) for OTP full refresh
    if (buffer1) {
      memset(buffer1, 0xFF, buffer1_size);
    }
    Adafruit_UC8151D::display(sleep);
  }
};

ThinkInk_102_Mono_GDEW0102I4FC display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
                                       EPD_BUSY, EPD_SPI);

void setup()
{
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  Serial.println();
  Serial.println(F("=================================================="));
  Serial.println(F(" Adafruit_EPD: GDEW0102I4FC on XIAO MG24          "));
  Serial.println(F("=================================================="));

  display.begin();

  Serial.println(F("Drawing test graphics in buffer..."));
  display.clearBuffer();

  // Header banner
  display.fillRect(0, 0, 80, 18, EPD_BLACK);
  display.setTextColor(EPD_WHITE);
  display.setTextSize(1);
  display.setCursor(6, 5);
  display.print("1.02\" MONO");

  // Status & specs
  display.setTextColor(EPD_BLACK);
  display.setCursor(4, 24);
  display.print("80 x 128 px");

  display.setCursor(4, 36);
  display.print("UC8175 EPD");

  display.setCursor(4, 48);
  display.print("XIAO + C102");

  display.drawFastHLine(4, 60, 72, EPD_BLACK);

  // Geometric shapes test
  display.drawRect(6, 66, 28, 22, EPD_BLACK);
  display.fillRect(10, 70, 20, 14, EPD_BLACK);

  display.drawCircle(58, 77, 11, EPD_BLACK);
  display.fillCircle(58, 77, 7, EPD_BLACK);

  display.drawFastHLine(4, 94, 72, EPD_BLACK);

  // Footer bar
  display.fillRect(0, 108, 80, 20, EPD_BLACK);
  display.setTextColor(EPD_WHITE);
  display.setCursor(4, 114);
  display.print("Adafruit_EPD");

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

