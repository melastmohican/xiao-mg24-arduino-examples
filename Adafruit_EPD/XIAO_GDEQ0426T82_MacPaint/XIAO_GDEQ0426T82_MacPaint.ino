/*****************************************************************************
* | File        :   XIAO_GDEQ0426T82_MacPaint.ino
* | Function    :   Drive Good Display GDEQ0426T82 with Adafruit_EPD (MacPaint Demo)
* | Info        :
*----------------
* Port of upiir's Seeed ePaper Partial Update "hello" animation
* for the 4.26" 800x480 Monochrome E-Paper Panel (GDEQ0426T82 / SSD1677)
* using Adafruit_EPD (ThinkInk_426_Grayscale4_GDEQ).
*
* Note: Custom MacPaint partial update demo.
* In Adafruit_SSD1677, displayPartial() retransmits the full 48 KB RAM buffer
* twice over SPI per update, which takes ~1.5 seconds per point.
*
* Board       : SiliconLabs:silabs:xiao_mg24:protocol_stack=none
* Panel       : Good Display GDEQ0426T82, 800x480, SSD1677 controller
* Connection  : Seeed ePaper Driver Board for XIAO v2
*               https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
*
* Pinout:
*   RST  -> D0
*   CS   -> D1
*   BUSY -> D2  (Active HIGH on SSD1677: 1 = busy, 0 = idle)
*   DC   -> D3
*   SCK  -> D8
*   MOSI -> D10
******************************************************************************/
#include "Adafruit_ThinkInk.h"
#include "panels/ThinkInk_426_Grayscale4_GDEQ.h"
#include "image.h"
#include "positions.h"

// Pin mapping for Seeed Studio ePaper Driver Board v2
#define EPD_RESET   D0
#define EPD_CS      D1
#define EPD_BUSY    D2
#define EPD_DC      D3
#define SRAM_CS     -1    // No external SRAM chip, use MCU RAM
#define EPD_SPI     &SPI

ThinkInk_426_Grayscale4_GDEQ display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY, EPD_SPI);

#define MACPAINT_X_OFFSET  80
#define MACPAINT_Y_OFFSET  0
#define MACPAINT_WIDTH     640
#define MACPAINT_HEIGHT    480

const int16_t BRUSH_RADIUS = 4;
int16_t animIndex = 0;
bool initialRenderDone = false;

void drawMacPaintBackground()
{
  Serial.println(F("[Adafruit] Performing initial full refresh with MacPaint canvas..."));
  display.clearBuffer();
  // Fill background with white
  display.fillRect(0, 0, display.width(), display.height(), EPD_WHITE);
  // Draw centered 640x480 MacPaint bitmap
  display.drawBitmap(MACPAINT_X_OFFSET, MACPAINT_Y_OFFSET,
                     gImage_macos_paint_640x480,
                     MACPAINT_WIDTH, MACPAINT_HEIGHT,
                     EPD_BLACK);
  display.display();
  Serial.println(F("[Adafruit] Full refresh complete. Ready for partial updates."));
}

void setup()
{
  Serial.begin(115200);
  uint32_t startMs = millis();
  while (!Serial && (millis() - startMs < 2000)) {
    // Bounded wait for serial
  }

  Serial.println(F("=================================================="));
  Serial.println(F(" XIAO MG24 + GDEQ0426T82 Adafruit_EPD Evaluation  "));
  Serial.println(F(" MacPaint Partial Refresh Handwriting Demo       "));
  Serial.println(F("=================================================="));

  display.begin(THINKINK_MONO);

  drawMacPaintBackground();
  initialRenderDone = true;
  delay(1000);
}

void loop()
{
  if (!initialRenderDone) return;

  if (animIndex < NUM_POSITIONS) {
    int16_t x0 = pgm_read_word(&positions[animIndex][0]);
    int16_t y0 = pgm_read_word(&positions[animIndex][1]);

    int16_t minX = x0 - BRUSH_RADIUS;
    int16_t maxX = x0 + BRUSH_RADIUS;
    int16_t minY = y0 - BRUSH_RADIUS;
    int16_t maxY = y0 + BRUSH_RADIUS;

    if (minX < 0) minX = 0;
    if (minY < 0) minY = 0;
    if (maxX >= 800) maxX = 799;
    if (maxY >= 480) maxY = 479;

    // Draw dot into framebuffer
    display.fillCircle(x0, y0, BRUSH_RADIUS, EPD_BLACK);

    // Call Adafruit partial display
    display.displayPartial(minX, minY, maxX, maxY);

    animIndex++;

    if (animIndex % 25 == 0 || animIndex >= NUM_POSITIONS) {
      Serial.print(F("[Adafruit] Drawing progress: "));
      Serial.print(animIndex);
      Serial.print(F(" / "));
      Serial.print(NUM_POSITIONS);
      Serial.println(F(" points"));
    }

    delay(20);
  } else {
    Serial.println(F("[Adafruit] Handwriting animation finished."));
    Serial.println(F("[Adafruit] Holding display for 8 seconds before restarting..."));
    delay(8000);

    drawMacPaintBackground();
    animIndex = 0;
    delay(1000);
  }
}
