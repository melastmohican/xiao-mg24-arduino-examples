// Demo.ino for GDEM037F51 on Seeed Studio XIAO MG24 + ePaper Driver Board v2
//
// 4-Color demo for Seeed Studio XIAO MG24 + ePaper Driver Board v2
//   - 3.7" 4-Color ePaper, 240 x 416
//   - Panel: GDEM037F51 / Waveshare 3.7inch e-Paper (G), SKU 31065, FPC-2303
//   - Driver IC: IST7163 (4-color: Black, White, Red, Yellow)
//   - Host MCU: XIAO MG24 (Silicon Labs EFR32MG24)
//   - Driver Board: Seeed Studio ePaper Driver Board for XIAO v2
//
// Pinout (from Seeed ePaper Driver Board v2 wiki):
//   https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
//   RST  -> D0
//   CS   -> D1
//   BUSY -> D2  (Active LOW on IST7163)
//   DC   -> D3
//   SCK  -> D8
//   MOSI -> D10

#include <SPI.h>
#include <GxEPD2_4C.h>
#include "GxEPD2_370c_GDEM037F51.h"
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeMono9pt7b.h>

// ===== Pin mapping for Seeed Studio ePaper Driver Board v2 =====
#define EPD_RST_PIN   D0
#define EPD_CS_PIN    D1
#define EPD_BUSY_PIN  D2
#define EPD_DC_PIN    D3
#define EPD_SCK_PIN   D8
#define EPD_MOSI_PIN  D10

// ===== Display Constructor =====
// Display: 3.7" 4-color (240x416) GDEM037F51 with IST7163 driver
GxEPD2_4C<GxEPD2_370c_GDEM037F51, GxEPD2_370c_GDEM037F51::HEIGHT> display(
  GxEPD2_370c_GDEM037F51(EPD_CS_PIN, EPD_DC_PIN, EPD_RST_PIN, EPD_BUSY_PIN)
);

// Convenience color names for the GDEM037F51 4-color palette
#define C_BLACK   GxEPD_BLACK
#define C_WHITE   GxEPD_WHITE
#define C_RED     GxEPD_RED
#define C_YELLOW  GxEPD_YELLOW

void showSplashScreen();
void showColorPalette();
void showColorTypography();
void showColorGeometry();
void showColorPatterns();
void showDashboard();

void setup()
{
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println(F("=================================================="));
  Serial.println(F(" GDEM037F51 4-Color Demo (XIAO MG24 + ePaper v2)  "));
  Serial.println(F("=================================================="));

  pinMode(EPD_RST_PIN, OUTPUT);
  pinMode(EPD_DC_PIN,  OUTPUT);
  pinMode(EPD_CS_PIN,  OUTPUT);

  SPI.begin();
  display.init(115200, true, 20, false);

  Serial.print(F("Display size: "));
  Serial.print(display.width());
  Serial.print(F("x"));
  Serial.println(display.height());
  Serial.println(F("Display initialized!"));
  Serial.println();

  const uint32_t PAGE_HOLD_MS = 5000;

  Serial.println(F("Screen 1: Splash"));
  showSplashScreen();
  delay(PAGE_HOLD_MS);

  Serial.println(F("Screen 2: Color Palette"));
  showColorPalette();
  delay(PAGE_HOLD_MS);

  Serial.println(F("Screen 3: Color Typography"));
  showColorTypography();
  delay(PAGE_HOLD_MS);

  Serial.println(F("Screen 4: Color Geometry"));
  showColorGeometry();
  delay(PAGE_HOLD_MS);

  Serial.println(F("Screen 5: Color Patterns"));
  showColorPatterns();
  delay(PAGE_HOLD_MS);

  Serial.println(F("Screen 6: Dashboard"));
  showDashboard();
  delay(PAGE_HOLD_MS);

  Serial.println(F("Demo complete. Hibernating display."));
  display.hibernate();
}

void loop()
{
  // Nothing to do in loop
}

// =====================================================================
// Helper: Draw centered text
// =====================================================================
void drawCenteredText(const char* text, int16_t y, const GFXfont* font)
{
  if (font) display.setFont(font);
  else display.setFont();
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);
  display.setCursor((display.width() - tbw) / 2 - tbx, y);
  display.print(text);
}

// =====================================================================
// Screen 1: Splash (240x416)
// =====================================================================
void showSplashScreen()
{
  display.setRotation(0);
  const uint16_t W = display.width();
  const uint16_t H = display.height();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    // Top colorful stripe
    int stripeH = 8, stripeY = 10;
    uint16_t colors[] = {C_RED, C_YELLOW, C_BLACK, C_RED};
    int stripeW = (W - 20) / 4;
    for (int i = 0; i < 4; i++) {
      display.fillRect(10 + i * stripeW, stripeY, stripeW, stripeH, colors[i]);
    }

    display.drawRect(10, 26, W - 20, H - 40, C_BLACK);

    display.setTextColor(C_BLACK);
    drawCenteredText("XIAO MG24 + ePaper", 60, &FreeSansBold9pt7b);

    display.setTextColor(C_RED);
    drawCenteredText("3.7\" Quad-Color Display", 90, &FreeSansBold9pt7b);

    display.drawFastHLine(W / 4, 105, W / 2, C_YELLOW);

    display.setTextColor(C_BLACK);
    drawCenteredText("GDEM037F51 / 3.7\" (G)", 140, &FreeSans9pt7b);
    drawCenteredText("240 x 416 Pixels", 170, &FreeSans9pt7b);
    drawCenteredText("IST7163 Controller", 200, &FreeSans9pt7b);

    // Color indicators
    int boxSize = 28;
    int gap = 16;
    int totalBoxesW = 4 * boxSize + 3 * gap;
    int startX = (W - totalBoxesW) / 2;
    int boxY = 240;

    display.fillRect(startX, boxY, boxSize, boxSize, C_BLACK);
    display.drawRect(startX, boxY, boxSize, boxSize, C_BLACK);

    display.fillRect(startX + (boxSize + gap), boxY, boxSize, boxSize, C_WHITE);
    display.drawRect(startX + (boxSize + gap), boxY, boxSize, boxSize, C_BLACK);

    display.fillRect(startX + 2 * (boxSize + gap), boxY, boxSize, boxSize, C_YELLOW);
    display.drawRect(startX + 2 * (boxSize + gap), boxY, boxSize, boxSize, C_BLACK);

    display.fillRect(startX + 3 * (boxSize + gap), boxY, boxSize, boxSize, C_RED);
    display.drawRect(startX + 3 * (boxSize + gap), boxY, boxSize, boxSize, C_BLACK);

    display.setTextColor(C_BLACK);
    drawCenteredText("Black - White - Yellow - Red", 300, &FreeSans9pt7b);

    display.drawFastHLine(W / 4, 320, W / 2, C_RED);

    display.setTextColor(C_BLACK);
    drawCenteredText("GxEPD2 Demo Suite", 355, &FreeSansBold9pt7b);
    drawCenteredText("Silicon Labs EFR32MG24", 380, &FreeSans9pt7b);

  } while (display.nextPage());
}

// =====================================================================
// Screen 2: Color Palette
// =====================================================================
void showColorPalette()
{
  display.setRotation(0);
  const uint16_t W = display.width();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    display.fillRect(0, 0, W, 32, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("Color Palette (4-Color)", 22, &FreeSansBold9pt7b);

    int bandH = 65;
    int startY = 45;

    // Black Band
    display.fillRect(15, startY, W - 30, bandH, C_BLACK);
    display.setTextColor(C_WHITE);
    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(25, startY + 40);
    display.print("BLACK (00b)");

    // White Band
    display.fillRect(15, startY + 75, W - 30, bandH, C_WHITE);
    display.drawRect(15, startY + 75, W - 30, bandH, C_BLACK);
    display.setTextColor(C_BLACK);
    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(25, startY + 75 + 40);
    display.print("WHITE (01b)");

    // Yellow Band
    display.fillRect(15, startY + 150, W - 30, bandH, C_YELLOW);
    display.drawRect(15, startY + 150, W - 30, bandH, C_BLACK);
    display.setTextColor(C_BLACK);
    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(25, startY + 150 + 40);
    display.print("YELLOW (10b)");

    // Red Band
    display.fillRect(15, startY + 225, W - 30, bandH, C_RED);
    display.setTextColor(C_WHITE);
    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(25, startY + 225 + 40);
    display.print("RED (11b)");

    // Bottom note
    display.setTextColor(C_BLACK);
    drawCenteredText("2 Bits Per Pixel - Native Colors", 395, &FreeSans9pt7b);

  } while (display.nextPage());
}

// =====================================================================
// Screen 3: Color Typography
// =====================================================================
void showColorTypography()
{
  display.setRotation(0);
  const uint16_t W = display.width();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    display.fillRect(0, 0, W, 32, C_RED);
    display.setTextColor(C_WHITE);
    drawCenteredText("Typography Test", 22, &FreeSansBold9pt7b);

    display.setFont(&FreeSansBold9pt7b);
    display.setTextColor(C_BLACK);
    display.setCursor(15, 65);
    display.print("FreeSans Bold 9pt");

    display.setFont(&FreeSans9pt7b);
    display.setTextColor(C_RED);
    display.setCursor(15, 95);
    display.print("Red text in FreeSans");

    display.setFont(&FreeSans9pt7b);
    display.setTextColor(C_BLACK);
    display.setCursor(15, 125);
    display.print("Yellow Highlight Box:");

    display.fillRect(15, 140, W - 30, 40, C_YELLOW);
    display.drawRect(15, 140, W - 30, 40, C_BLACK);
    display.setTextColor(C_BLACK);
    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(25, 166);
    display.print("Black on Yellow");

    display.fillRect(15, 195, W - 30, 40, C_BLACK);
    display.setTextColor(C_YELLOW);
    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(25, 221);
    display.print("Yellow on Black");

    display.fillRect(15, 250, W - 30, 40, C_RED);
    display.setTextColor(C_WHITE);
    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(25, 276);
    display.print("White on Red");

    display.setFont(&FreeMono9pt7b);
    display.setTextColor(C_BLACK);
    display.setCursor(15, 325);
    display.print("Mono: ABC abc 123");

    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(C_RED);
    display.setCursor(15, 355);
    display.print("IST7163 @ 240x416");

    display.setFont(&FreeSans9pt7b);
    display.setTextColor(C_BLACK);
    display.setCursor(15, 395);
    display.print("Clean electrophoretic text");

  } while (display.nextPage());
}

// =====================================================================
// Screen 4: Color Geometry
// =====================================================================
void showColorGeometry()
{
  display.setRotation(0);
  const uint16_t W = display.width();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    display.fillRect(0, 0, W, 32, C_YELLOW);
    display.drawRect(0, 0, W, 32, C_BLACK);
    display.setTextColor(C_BLACK);
    drawCenteredText("Geometric Primitives", 22, &FreeSansBold9pt7b);

    // Concentric circles
    int cx = 70, cy = 110;
    display.fillCircle(cx, cy, 45, C_RED);
    display.fillCircle(cx, cy, 32, C_YELLOW);
    display.fillCircle(cx, cy, 20, C_BLACK);
    display.fillCircle(cx, cy, 8, C_WHITE);

    // Nested squares
    int sqX = 145, sqY = 65, sqS = 70;
    display.fillRect(sqX, sqY, sqS, sqS, C_BLACK);
    display.fillRect(sqX + 10, sqY + 10, sqS - 20, sqS - 20, C_YELLOW);
    display.fillRect(sqX + 20, sqY + 20, sqS - 40, sqS - 40, C_RED);
    display.fillRect(sqX + 28, sqY + 28, sqS - 56, sqS - 56, C_WHITE);

    // Diagonal crosses
    display.drawRect(15, 180, W - 30, 80, C_BLACK);
    display.drawLine(15, 180, W - 15, 260, C_RED);
    display.drawLine(15, 260, W - 15, 180, C_RED);
    display.drawLine(W / 2, 180, W / 2, 260, C_YELLOW);
    display.drawLine(15, 220, W - 15, 220, C_YELLOW);

    // Triangles
    display.fillTriangle(30, 360, 80, 280, 130, 360, C_RED);
    display.drawTriangle(30, 360, 80, 280, 130, 360, C_BLACK);

    display.fillTriangle(140, 360, 185, 280, 230, 360, C_YELLOW);
    display.drawTriangle(140, 360, 185, 280, 230, 360, C_BLACK);

    display.setTextColor(C_BLACK);
    drawCenteredText("Concentric & Polygons", 395, &FreeSans9pt7b);

  } while (display.nextPage());
}

// =====================================================================
// Screen 5: Color Patterns
// =====================================================================
void showColorPatterns()
{
  display.setRotation(0);
  const uint16_t W = display.width();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    display.fillRect(0, 0, W, 32, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("Patterns & Textures", 22, &FreeSansBold9pt7b);

    // Checkerboard
    int box = 15;
    for (int y = 45; y < 140; y += box) {
      for (int x = 15; x < W - 15; x += box) {
        int idx = ((x - 15) / box + (y - 45) / box) % 3;
        uint16_t c = (idx == 0) ? C_BLACK : ((idx == 1) ? C_YELLOW : C_RED);
        display.fillRect(x, y, box, box, c);
      }
    }

    // Vertical striped pattern
    int vY = 160;
    int vH = 90;
    for (int x = 15; x < W - 15; x += 12) {
      display.fillRect(x, vY, 3, vH, C_BLACK);
      display.fillRect(x + 3, vY, 3, vH, C_YELLOW);
      display.fillRect(x + 6, vY, 3, vH, C_RED);
      display.fillRect(x + 9, vY, 3, vH, C_WHITE);
    }

    // Horizontal striped bands
    int hY = 270;
    for (int i = 0; i < 9; i++) {
      uint16_t c = (i % 3 == 0) ? C_RED : ((i % 3 == 1) ? C_YELLOW : C_BLACK);
      display.fillRect(15, hY + i * 11, W - 30, 9, c);
    }

    display.setTextColor(C_BLACK);
    drawCenteredText("4-Color Dither & Striping", 395, &FreeSans9pt7b);

  } while (display.nextPage());
}

// =====================================================================
// Screen 6: Dashboard
// =====================================================================
void showDashboard()
{
  display.setRotation(0);
  const uint16_t W = display.width();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    // Header bar
    display.fillRect(0, 0, W, 40, C_BLACK);
    display.setTextColor(C_WHITE);
    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(12, 26);
    display.print("SENSOR DASHBOARD");

    display.fillRect(W - 40, 10, 28, 20, C_RED);
    display.setTextColor(C_WHITE);
    display.setFont(&FreeSans9pt7b);
    display.setCursor(W - 35, 25);
    display.print("4C");

    // Card 1: Temperature
    display.drawRect(10, 50, W - 20, 75, C_BLACK);
    display.fillRect(10, 50, 6, 75, C_RED);
    display.setFont(&FreeSans9pt7b);
    display.setTextColor(C_BLACK);
    display.setCursor(24, 75);
    display.print("Temperature");
    display.setFont(&FreeSansBold9pt7b);
    display.setTextColor(C_RED);
    display.setCursor(24, 105);
    display.print("24.8 C");
    display.setFont(&FreeSans9pt7b);
    display.setTextColor(C_BLACK);
    display.setCursor(140, 105);
    display.print("Target: 22.0");

    // Card 2: Humidity
    display.drawRect(10, 135, W - 20, 75, C_BLACK);
    display.fillRect(10, 135, 6, 75, C_YELLOW);
    display.setFont(&FreeSans9pt7b);
    display.setTextColor(C_BLACK);
    display.setCursor(24, 160);
    display.print("Relative Humidity");
    display.setFont(&FreeSansBold9pt7b);
    display.setTextColor(C_BLACK);
    display.setCursor(24, 190);
    display.print("58.2 %");
    display.setFont(&FreeSans9pt7b);
    display.setTextColor(C_BLACK);
    display.setCursor(140, 190);
    display.print("Normal: 40-60");

    // Card 3: Air Quality Status
    display.drawRect(10, 220, W - 20, 75, C_BLACK);
    display.fillRect(10, 220, 6, 75, C_BLACK);
    display.setFont(&FreeSans9pt7b);
    display.setTextColor(C_BLACK);
    display.setCursor(24, 245);
    display.print("Air Quality Index");
    display.setFont(&FreeSansBold9pt7b);
    display.setTextColor(C_BLACK);
    display.setCursor(24, 275);
    display.print("AQI 42");

    display.fillRect(125, 255, 80, 26, C_YELLOW);
    display.drawRect(125, 255, 80, 26, C_BLACK);
    display.setFont(&FreeSans9pt7b);
    display.setTextColor(C_BLACK);
    display.setCursor(138, 273);
    display.print("GOOD");

    // Progress bar for battery
    display.drawRect(10, 310, W - 20, 50, C_BLACK);
    display.setFont(&FreeSans9pt7b);
    display.setTextColor(C_BLACK);
    display.setCursor(24, 330);
    display.print("Battery (3.3V Line)");

    // Battery bar
    int barX = 24, barY = 338, barW = W - 48, barH = 14;
    display.drawRect(barX, barY, barW, barH, C_BLACK);
    display.fillRect(barX + 2, barY + 2, (barW - 4) * 85 / 100, barH - 4, C_YELLOW);

    // Footer info
    display.setFont(&FreeSans9pt7b);
    display.setTextColor(C_BLACK);
    display.setCursor(15, 385);
    display.print("Host: Seeed XIAO MG24");
    display.setCursor(15, 405);
    display.setTextColor(C_RED);
    display.print("Panel: Good Display GDEM037F51");

  } while (display.nextPage());
}
