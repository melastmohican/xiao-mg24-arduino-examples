// Demo.ino for GDEY029F52 on Seeed Studio XIAO MG24 + ePaper Driver Board v2
//
// Six-screen 4-color demo for Seeed Studio XIAO MG24 + ePaper Driver Board v2
//   - 2.9" 4-Color ePaper, 296 x 128 (128 x 296 native)
//   - Panel: GDEY029F52 (JD79661 driver, 4-color: Black, White, Red, Yellow)
//   - Driver in GxEPD2: in-sketch GxEPD2_290c_GDEY029F52 (no stock class exists)
//   - Host MCU: XIAO MG24 (Silicon Labs EFR32MG24)
//   - Driver Board: Seeed Studio ePaper Driver Board for XIAO v2
//
// Pinout (from Seeed ePaper Driver Board v2 wiki):
//   https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
//   RST  -> D0
//   CS   -> D1
//   BUSY -> D2  (Active LOW on JD79661: 0 = busy, 1 = ready/idle)
//   DC   -> D3
//   SCK  -> D8
//   MOSI -> D10
//

#include <SPI.h>
#include <GxEPD2_4C.h>
#include "GxEPD2_290c_GDEY029F52.h"
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

// The class is 128 wide x 296 tall natively (rotation 0 = portrait).
// Rotation 1 gives 296x128 landscape; use 3 for the same view rotated 180 degrees.
// All screens below are laid out for the 296x128 landscape canvas.
#define DEMO_ROTATION 1

// ===== Display Constructor =====
// Display: 2.9" 4-color (128x296) with JD79661 driver; one full-height page (9,472 bytes)
GxEPD2_4C<GxEPD2_290c_GDEY029F52, GxEPD2_290c_GDEY029F52::HEIGHT> display(
  GxEPD2_290c_GDEY029F52(EPD_CS_PIN, EPD_DC_PIN, EPD_RST_PIN, EPD_BUSY_PIN)
);

// Convenience color names for the GDEY029F52 4-color palette
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
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  Serial.println();
  Serial.println(F("=================================================="));
  Serial.println(F(" GDEY029F52 4-Color Demo (XIAO MG24 + ePaper Board v2)"));
  Serial.println(F("=================================================="));

  pinMode(EPD_RST_PIN, OUTPUT);
  pinMode(EPD_DC_PIN,  OUTPUT);
  pinMode(EPD_CS_PIN,  OUTPUT);

  SPI.begin();
  display.init(115200, true, 20, false);

  display.setRotation(DEMO_ROTATION);
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

// Helper: draw text centered inside a horizontal span [x, x+w)
void drawTextInSpan(const char* text, int16_t x, int16_t w, int16_t y, const GFXfont* font)
{
  if (font) display.setFont(font);
  else display.setFont();
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);
  display.setCursor(x + (w - tbw) / 2 - tbx, y);
  display.print(text);
}

// =====================================================================
// Screen 1: Splash
// =====================================================================
void showSplashScreen()
{
  display.setRotation(DEMO_ROTATION);
  const uint16_t W = display.width();
  const uint16_t H = display.height();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    // Colorful top stripe: 4 native colors
    int stripeH = 6, stripeY = 4;
    uint16_t colors[] = {C_RED, C_YELLOW, C_BLACK, C_RED};
    int stripeW = (W - 10) / 4;
    for (int i = 0; i < 4; i++) {
      display.fillRect(5 + i * stripeW, stripeY, stripeW, stripeH, colors[i]);
    }

    display.drawRect(4, 14, W - 8, H - 22, C_BLACK);

    display.setTextColor(C_BLACK);
    drawCenteredText("XIAO MG24 + ePaper", 38, &FreeSansBold9pt7b);

    display.setTextColor(C_RED);
    drawCenteredText("Quad-Color e-Paper", 58, &FreeSansBold9pt7b);

    display.drawFastHLine(W / 4, 68, W / 2, C_YELLOW);

    display.setTextColor(C_BLACK);
    drawCenteredText("GDEY029F52 (296x128)", 88, &FreeSans9pt7b);

    display.setTextColor(C_RED);
    drawCenteredText("Seeed ePaper v2", 106, &FreeSans9pt7b);

    // Bottom colorful stripe
    uint16_t bottomColors[] = {C_BLACK, C_YELLOW, C_RED, C_BLACK};
    for (int i = 0; i < 4; i++) {
      display.fillRect(5 + i * stripeW, H - 7, stripeW, stripeH, bottomColors[i]);
    }
  } while (display.nextPage());
}

// =====================================================================
// Screen 2: Color Palette
// =====================================================================
void showColorPalette()
{
  display.setRotation(DEMO_ROTATION);
  const uint16_t W = display.width();
  const uint16_t H = display.height();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    // Header bar
    display.fillRect(0, 0, W, 22, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("4-Color Palette", 16, &FreeSansBold9pt7b);

    // 4 swatches across the width
    const uint16_t swatchColors[] = {C_BLACK, C_WHITE, C_RED, C_YELLOW};
    const char* names[] = {"BLACK", "WHITE", "RED", "YELLOW"};
    const int sw = 60, sh = 38, gap = 10;
    const int sx = (W - (4 * sw + 3 * gap)) / 2;
    const int sy = 26;

    for (int i = 0; i < 4; i++) {
      int x = sx + i * (sw + gap);
      display.fillRoundRect(x, sy, sw, sh, 4, swatchColors[i]);
      display.drawRoundRect(x, sy, sw, sh, 4, C_BLACK);
      display.setTextColor(swatchColors[i] == C_BLACK || swatchColors[i] == C_RED ? C_WHITE : C_BLACK);
      drawTextInSpan(names[i], x, sw, sy + 15, nullptr);
    }

    // Color combination badges
    const int row2Y = sy + sh + 6;
    uint16_t bgColors[] = {C_RED, C_YELLOW, C_BLACK, C_RED};
    uint16_t fgColors[] = {C_YELLOW, C_RED, C_WHITE, C_BLACK};
    for (int i = 0; i < 4; i++) {
      int x = sx + i * (sw + gap);
      display.fillRoundRect(x, row2Y, sw, 22, 4, bgColors[i]);
      display.fillCircle(x + sw / 2, row2Y + 11, 8, fgColors[i]);
    }

    // Full-width color bars at the bottom
    const int barY = row2Y + 28;
    uint16_t barColors[] = {C_BLACK, C_RED, C_YELLOW, C_WHITE};
    const int barW = W / 4;
    for (int i = 0; i < 4; i++) {
      display.fillRect(i * barW, barY, barW, H - barY, barColors[i]);
      if (barColors[i] == C_WHITE) {
        display.drawRect(i * barW, barY, barW, H - barY, C_BLACK);
      }
    }
  } while (display.nextPage());
}

// =====================================================================
// Screen 3: Color Typography
// =====================================================================
void showColorTypography()
{
  display.setRotation(DEMO_ROTATION);
  const uint16_t W = display.width();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    // Header bar
    display.fillRect(0, 0, W, 22, C_RED);
    display.setTextColor(C_WHITE);
    drawCenteredText("Color Typography", 16, &FreeSansBold9pt7b);

    int y = 44;
    display.setFont(&FreeSansBold9pt7b);

    display.setTextColor(C_BLACK);
    display.setCursor(12, y); display.print("Black");
    display.setTextColor(C_RED);
    display.setCursor(100, y); display.print("Red");

    // Yellow text inside a black box for contrast
    display.fillRect(180, y - 14, 100, 20, C_BLACK);
    display.setTextColor(C_YELLOW);
    drawTextInSpan("Yellow", 180, 100, y, &FreeSansBold9pt7b);

    y += 26;
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(C_RED);
    display.setCursor(12, y); display.print("GDEY029F52 QuadColor");

    y += 14;
    display.drawFastHLine(12, y, W - 24, C_BLACK);

    // Color text badges
    y += 8;
    const int bw = 132, bh = 24;
    display.fillRoundRect(12, y, bw, bh, 4, C_RED);
    display.setTextColor(C_WHITE);
    drawTextInSpan("White on Red", 12, bw, y + 17, &FreeSans9pt7b);

    display.fillRoundRect(W - 12 - bw, y, bw, bh, 4, C_BLACK);
    display.setTextColor(C_YELLOW);
    drawTextInSpan("Yellow on Black", W - 12 - bw, bw, y + 17, &FreeSans9pt7b);
  } while (display.nextPage());
}

// =====================================================================
// Screen 4: Color Geometry
// =====================================================================
void showColorGeometry()
{
  display.setRotation(DEMO_ROTATION);
  const uint16_t W = display.width();
  const uint16_t H = display.height();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    // Top header bar
    display.fillRect(0, 0, W, 18, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("Color Geometry", 14, &FreeSansBold9pt7b);

    // The shape groups were laid out for 250x122; center them on the 296x128 canvas.
    const int ox = (W - 250) / 2;
    const int oy = (H - 122) / 2;

    // 1. Cascading rectangles
    uint16_t rcColors[] = {C_RED, C_YELLOW, C_BLACK};
    for (int i = 0; i < 3; i++) {
      display.fillRect(ox + 8 + i * 12, oy + 26 + i * 10, 42, 26, rcColors[i]);
      display.drawRect(ox + 8 + i * 12, oy + 26 + i * 10, 42, 26, C_BLACK);
    }

    // 2. Circles and triangles
    display.fillCircle(ox + 88,  oy + 36, 10, C_RED);    display.drawCircle(ox + 88,  oy + 36, 10, C_BLACK);
    display.fillCircle(ox + 110, oy + 36, 10, C_YELLOW); display.drawCircle(ox + 110, oy + 36, 10, C_BLACK);
    display.fillCircle(ox + 99,  oy + 52, 10, C_BLACK);

    display.fillTriangle(ox + 76, oy + 88, ox + 88, oy + 66, ox + 100, oy + 88, C_RED);
    display.drawTriangle(ox + 76, oy + 88, ox + 88, oy + 66, ox + 100, oy + 88, C_BLACK);
    display.fillTriangle(ox + 98, oy + 88, ox + 110, oy + 66, ox + 122, oy + 88, C_YELLOW);
    display.drawTriangle(ox + 98, oy + 88, ox + 110, oy + 66, ox + 122, oy + 88, C_BLACK);

    // 3. Concentric circles
    display.drawCircle(ox + 160, oy + 56, 26, C_BLACK);
    display.fillCircle(ox + 160, oy + 56, 19, C_RED);
    display.fillCircle(ox + 160, oy + 56, 12, C_YELLOW);
    display.fillCircle(ox + 160, oy + 56, 5,  C_BLACK);

    // 4. 2x2 color swatch grid
    const int gx = ox + 196, gy = oy + 30;
    display.fillRect(gx,      gy,      20, 20, C_RED);
    display.fillRect(gx + 22, gy,      20, 20, C_YELLOW);
    display.fillRect(gx,      gy + 22, 20, 20, C_BLACK);
    display.fillRect(gx + 22, gy + 22, 20, 20, C_WHITE);
    display.drawRect(gx + 22, gy + 22, 20, 20, C_BLACK);
    display.drawRect(gx - 2,  gy - 2,  46, 46, C_BLACK);

    // Footer label
    display.setTextColor(C_BLACK);
    drawCenteredText("GFX Primitives", H - 8, &FreeSans9pt7b);
  } while (display.nextPage());
}

// =====================================================================
// Screen 5: Color Patterns
// =====================================================================
void showColorPatterns()
{
  display.setRotation(DEMO_ROTATION);
  const uint16_t W = display.width();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    display.fillRect(0, 0, W, 22, C_RED);
    display.setTextColor(C_WHITE);
    drawCenteredText("Color Patterns", 16, &FreeSansBold9pt7b);

    const int bw = 50, bh = 50;
    const int gapX = (W - 4 * bw) / 5;  // even spacing across the width
    uint16_t pColors[] = {C_RED, C_YELLOW, C_BLACK, C_WHITE};
    const int by = 28;

    // 1. Checkerboard
    int bx1 = gapX;
    for (int py = 0; py < bh / 10; py++) {
      for (int px = 0; px < bw / 10; px++) {
        display.fillRect(bx1 + px * 10, by + py * 10, 10, 10, pColors[(px + py) % 4]);
      }
    }
    display.drawRect(bx1, by, bw, bh, C_BLACK);

    // 2. Horizontal stripes
    int bx2 = bx1 + bw + gapX;
    for (int py = 0; py < bh; py += 10) {
      display.fillRect(bx2, by + py, bw, 10, pColors[(py / 10) % 4]);
    }
    display.drawRect(bx2, by, bw, bh, C_BLACK);

    // 3. Vertical stripes
    int bx3 = bx2 + bw + gapX;
    for (int px = 0; px < bw; px += 10) {
      display.fillRect(bx3 + px, by, 10, bh, pColors[(px / 10) % 4]);
    }
    display.drawRect(bx3, by, bw, bh, C_BLACK);

    // 4. Dot grid
    int bx4 = bx3 + bw + gapX;
    for (int py = 0; py < bh; py += 12) {
      for (int px = 0; px < bw; px += 12) {
        int ci = (px / 12 + py / 12) % 3;
        display.fillCircle(bx4 + px + 6, by + py + 6, 4, pColors[ci]);
      }
    }
    display.drawRect(bx4, by, bw, bh, C_BLACK);

    // Full-width color bar sequence at the bottom
    const int barY = 86;
    const int barH = 9;
    for (int i = 0; i < 4; i++) {
      display.fillRect(10, barY + i * (barH + 1), W - 20, barH, pColors[i]);
      if (pColors[i] == C_WHITE) {
        display.drawRect(10, barY + i * (barH + 1), W - 20, barH, C_BLACK);
      }
    }
  } while (display.nextPage());
}

// =====================================================================
// Screen 6: Dashboard
// =====================================================================
void drawColorCard(int x, int y, int w, int h, const char* title,
                   const char* value, const char* unit, uint16_t accent)
{
  display.drawRoundRect(x, y, w, h, 4, C_BLACK);

  // Header banner inside card (height 14px)
  uint16_t headerBg = accent;
  uint16_t headerText = (accent == C_YELLOW || accent == C_WHITE) ? C_BLACK : C_WHITE;

  display.fillRoundRect(x + 1, y + 1, w - 2, 14, 3, headerBg);
  if (accent == C_WHITE) {
    display.drawFastHLine(x + 1, y + 14, w - 2, C_BLACK);
  }

  // Header title (standard 5x7 font: setCursor Y is the TOP edge)
  display.setTextColor(headerText);
  drawTextInSpan(title, x, w, y + 4, nullptr);

  // Value in FreeSansBold9pt7b (setCursor Y is the BASELINE)
  display.setTextColor(accent == C_WHITE ? C_BLACK : (accent == C_YELLOW ? C_BLACK : accent));
  display.setFont(&FreeSansBold9pt7b);
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(value, 0, 0, &tbx, &tby, &tbw, &tbh);
  int valX = x + 8 - tbx;
  int valY = y + 32;
  display.setCursor(valX, valY);
  display.print(value);

  // Unit text (standard 5x7 font: setCursor Y is the TOP edge)
  display.setTextColor(C_BLACK);
  display.setFont();
  display.setCursor(valX + tbw + 6, y + 24);
  display.print(unit);
}

void showDashboard()
{
  display.setRotation(DEMO_ROTATION);
  const uint16_t W = display.width();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    // Header bar
    display.fillRect(0, 0, W, 16, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("System Dashboard", 12, &FreeSansBold9pt7b);

    // 2x2 grid of cards
    const int cw = (W - 18) / 2, ch = 40;
    const int col1X = 6, col2X = 6 + cw + 6;
    const int row1Y = 20, row2Y = 64;

    char uptBuf[16]; snprintf(uptBuf, sizeof(uptBuf), "%lus", millis() / 1000);

    drawColorCard(col1X, row1Y, cw, ch, "TEMPERATURE", "23.5", "degC", C_RED);
    drawColorCard(col2X, row1Y, cw, ch, "HUMIDITY",    "65%",  "RH",   C_BLACK);
    drawColorCard(col1X, row2Y, cw, ch, "BATTERY",     "4.1V", "LiPo", C_YELLOW);
    drawColorCard(col2X, row2Y, cw, ch, "UPTIME",      uptBuf, "running", C_RED);

    // Bottom status and progress bar
    const int barY = 110;
    display.setFont();
    display.setTextColor(C_BLACK);
    display.setCursor(6, barY + 2);
    display.print("Status:");

    const int barX = 52, barW = 190, barH = 12;
    display.drawRect(barX, barY, barW, barH, C_BLACK);
    uint16_t barColors[] = {C_RED, C_YELLOW, C_BLACK, C_RED};
    int segW = (barW - 2) / 4;
    for (int i = 0; i < 4; i++) {
      display.fillRect(barX + 1 + i * segW, barY + 1, segW, barH - 2, barColors[i]);
    }

    display.setTextColor(C_BLACK);
    display.setCursor(barX + barW + 6, barY + 2);
    display.print("100%");
  } while (display.nextPage());
}
