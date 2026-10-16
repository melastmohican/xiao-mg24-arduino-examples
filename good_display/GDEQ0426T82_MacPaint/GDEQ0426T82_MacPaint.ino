/*****************************************************************************
* | File        :   GDEQ0426T82_MacPaint.ino
* | Function    :   Drive Good Display GDEQ0426T82 via Direct Hardware SPI (MacPaint Demo)
* | Info        :
*----------------
* Port of upiir's Seeed ePaper Partial Update "hello" animation
* for the 4.26" 800x480 Monochrome E-Paper Panel (GDEQ0426T82 / SSD1677)
* using direct Good Display bare-metal register commands.
*
* Note: This is a custom MacPaint partial update demo, not a standard
* manufacturer demo package.
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
#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "image.h"
#include "positions.h"

#define MACPAINT_X_OFFSET  80
#define MACPAINT_Y_OFFSET  0
#define MACPAINT_WIDTH     640
#define MACPAINT_HEIGHT    480

const int16_t BRUSH_RADIUS = 4;

// Vertical flip knob to align with the panel's physical mounting (Y-inverted)
#define EPD_FLIP_Y 1

#if EPD_FLIP_Y
  #define MAP_X(x) (x)
  #define MAP_Y(y) (EPD_HEIGHT - 1 - (y))
#else
  #define MAP_X(x) (x)
  #define MAP_Y(y) (y)
#endif

// 48 KB 1-bit frame buffer in RAM (800 x 480 / 8 bytes)
// Panel polarity: 1 = white, 0 = black
static uint8_t epd_buffer[EPD_ARRAY];

int16_t animIndex = 0;
bool initialRenderDone = false;

void drawMacPaintBackground()
{
  Serial.println(F("[GD] Initializing 48 KB buffer with MacPaint canvas..."));
  memset(epd_buffer, 0xFF, sizeof(epd_buffer));

  // Copy 640x480 MacPaint bitmap into epd_buffer centered at x=80
  for (uint16_t row = 0; row < MACPAINT_HEIGHT; row++) {
    for (uint16_t col = 0; col < MACPAINT_WIDTH; col++) {
      uint8_t byteVal = pgm_read_byte(&gImage_macos_paint_640x480[row * (MACPAINT_WIDTH / 8) + (col / 8)]);
      bool isBlack = (byteVal & (0x80 >> (col % 8))) != 0;
      if (isBlack) {
        uint16_t sx = MACPAINT_X_OFFSET + col;
        uint16_t sy = MACPAINT_Y_OFFSET + row;
        uint16_t bx = MAP_X(sx);
        uint16_t by = MAP_Y(sy);
        epd_buffer[(uint32_t)by * (EPD_WIDTH / 8) + (bx / 8)] &= ~(0x80 >> (bx % 8)); // 0 = black
      }
    }
  }

  Serial.println(F("[GD] Sending initial full refresh to SSD1677..."));
  EPD_Full_Update_From_Buffer(epd_buffer);
  Serial.println(F("[GD] Full refresh complete. Ready for partial updates."));
}

void setup()
{
  Serial.begin(115200);
  uint32_t startMs = millis();
  while (!Serial && (millis() - startMs < 2000)) {
    // Bounded wait for serial
  }

  Serial.println(F("=================================================="));
  Serial.println(F(" XIAO MG24 + GDEQ0426T82 Good Display Direct Demo "));
  Serial.println(F(" MacPaint Partial Refresh Handwriting Demo       "));
  Serial.println(F("=================================================="));

  pinMode(EPD_BUSY_PIN, INPUT);
  pinMode(EPD_RST_PIN, OUTPUT);
  pinMode(EPD_DC_PIN, OUTPUT);
  pinMode(EPD_CS_PIN, OUTPUT);

  EPD_SPI_PORT.begin();

  Serial.println(F("[GD] Initializing SSD1677 hardware..."));
  EPD_HW_Init();

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

    // Draw filled circle into epd_buffer (0 = black)
    for (int16_t dy = -BRUSH_RADIUS; dy <= BRUSH_RADIUS; dy++) {
      for (int16_t dx = -BRUSH_RADIUS; dx <= BRUSH_RADIUS; dx++) {
        if (dx * dx + dy * dy <= BRUSH_RADIUS * BRUSH_RADIUS) {
          int16_t px = x0 + dx;
          int16_t py = y0 + dy;
          if (px >= 0 && px < EPD_WIDTH && py >= 0 && py < EPD_HEIGHT) {
            int16_t bx = MAP_X(px);
            int16_t by = MAP_Y(py);
            epd_buffer[(uint32_t)by * (EPD_WIDTH / 8) + (bx / 8)] &= ~(0x80 >> (bx % 8));
          }
        }
      }
    }

    // Compute bounding box in mapped buffer coordinates
    int16_t rawMinX = x0 - BRUSH_RADIUS;
    int16_t rawMaxX = x0 + BRUSH_RADIUS;
    int16_t rawMinY = y0 - BRUSH_RADIUS;
    int16_t rawMaxY = y0 + BRUSH_RADIUS;

    int16_t bMinX = MAP_X(rawMaxX);
    int16_t bMaxX = MAP_X(rawMinX);
    int16_t bMinY = MAP_Y(rawMaxY);
    int16_t bMaxY = MAP_Y(rawMinY);

    if (bMinX > bMaxX) { int16_t t = bMinX; bMinX = bMaxX; bMaxX = t; }
    if (bMinY > bMaxY) { int16_t t = bMinY; bMinY = bMaxY; bMaxY = t; }

    if (bMinX < 0) bMinX = 0;
    if (bMinY < 0) bMinY = 0;
    if (bMaxX >= EPD_WIDTH) bMaxX = EPD_WIDTH - 1;
    if (bMaxY >= EPD_HEIGHT) bMaxY = EPD_HEIGHT - 1;

    int16_t startByteX = bMinX / 8;
    int16_t endByteX = bMaxX / 8;
    int16_t bytesPerRow = endByteX - startByteX + 1;
    int16_t alignedX = startByteX * 8;
    int16_t alignedW = bytesPerRow * 8;
    int16_t boxH = bMaxY - bMinY + 1;

    // Send only the 18-byte bounding box to SSD1677 and trigger 0xFC partial update
    EPD_Part_Update_From_Buffer(alignedX, bMinY, alignedW, boxH, epd_buffer);

    animIndex++;

    if (animIndex % 25 == 0 || animIndex >= NUM_POSITIONS) {
      Serial.print(F("[GD] Drawing progress: "));
      Serial.print(animIndex);
      Serial.print(F(" / "));
      Serial.print(NUM_POSITIONS);
      Serial.println(F(" points"));
    }

    delay(20);
  } else {
    Serial.println(F("[GD] Handwriting animation finished."));
    Serial.println(F("[GD] Holding display for 8 seconds before restarting..."));
    delay(8000);

    drawMacPaintBackground();
    animIndex = 0;
    delay(1000);
  }
}
