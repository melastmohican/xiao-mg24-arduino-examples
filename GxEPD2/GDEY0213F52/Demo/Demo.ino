// Demo.ino for GDEY0213F52 on Seeed Studio XIAO MG24 + ePaper Driver Board v2
//
// Comprehensive demo for Seeed Studio XIAO MG24 + ePaper Driver Board v2
//   - 2.13" 4-Color ePaper, 250 x 122
//   - Panel: GDEY0213F52 (JD79676A driver, 4-color: Black, White, Red, Yellow)
//   - Host MCU: XIAO MG24 (Silicon Labs EFR32MG24)
//   - Driver Board: Seeed Studio ePaper Driver Board for XIAO v2
//
// Pinout (from Seeed ePaper Driver Board v2 wiki):
//   https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
//   RST  -> D0
//   CS   -> D1
//   BUSY -> D2  (Active LOW on JD79676A: 0 = busy, 1 = ready/idle)
//   DC   -> D3
//   SCK  -> D8
//   MOSI -> D10
//

#include <SPI.h>
#include <GxEPD2_4C.h>
#include "GxEPD2_213c_GDEY0213F52.h"
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

// Landscape 250x122. Flip to 3 if the panel shows content upside down.
#define DEMO_ROTATION 1

// ===== Display Constructor =====
// Display: 2.13" 4-color (122x250) with JD79676A driver
GxEPD2_4C<GxEPD2_213c_GDEY0213F52, GxEPD2_213c_GDEY0213F52::HEIGHT> display(
  GxEPD2_213c_GDEY0213F52(EPD_CS_PIN, EPD_DC_PIN, EPD_RST_PIN, EPD_BUSY_PIN)
);

// Convenience color names for the GDEY0213F52 4-color palette
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
  Serial.println(F(" GDEY0213F52 4-Color Demo (XIAO MG24 + ePaper Board v2)"));
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
// Screen 1: Splash
// =====================================================================
void showSplashScreen()
{
  display.setRotation(DEMO_ROTATION); // Landscape 250x122
  const uint16_t W = display.width();
  const uint16_t H = display.height();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    // Colorful top stripe — 4 native colors
    int stripeH = 6, stripeY = 4;
    uint16_t colors[] = {C_RED, C_YELLOW, C_BLACK, C_RED};
    int stripeW = (W - 10) / 4;
    for (int i = 0; i < 4; i++) {
      display.fillRect(5 + i * stripeW, stripeY, stripeW, stripeH, colors[i]);
    }

    display.drawRect(4, 14, W - 8, H - 22, C_BLACK);

    display.setTextColor(C_BLACK);
    drawCenteredText("XIAO MG24 + ePaper", 34, &FreeSansBold9pt7b);

    display.setTextColor(C_RED);
    drawCenteredText("Quad-Color e-Paper", 54, &FreeSansBold9pt7b);

    display.drawFastHLine(W / 4, 63, W / 2, C_YELLOW);

    display.setTextColor(C_BLACK);
    drawCenteredText("GDEY0213F52 (250x122)", 80, &FreeSans9pt7b);

    display.setTextColor(C_RED);
    drawCenteredText("Seeed ePaper v2", 98, &FreeSans9pt7b);

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

    // 4 Swatches across width
    const uint16_t swatchColors[] = {C_BLACK, C_WHITE, C_RED, C_YELLOW};
    const char* names[] = {"BLACK", "WHITE", "RED", "YELLOW"};
    int sw = 52, sh = 38, gap = 6;
    int sx = (W - (4 * sw + 3 * gap)) / 2;
    int sy = 26;

    for (int i = 0; i < 4; i++) {
      int x = sx + i * (sw + gap);
      display.fillRoundRect(x, sy, sw, sh, 4, swatchColors[i]);
      display.drawRoundRect(x, sy, sw, sh, 4, C_BLACK);
      display.setFont();
      display.setTextColor(swatchColors[i] == C_BLACK || swatchColors[i] == C_RED ? C_WHITE : C_BLACK);
      int16_t tbx, tby; uint16_t tbw, tbh;
      display.getTextBounds(names[i], 0, 0, &tbx, &tby, &tbw, &tbh);
      display.setCursor(x + (sw - tbw) / 2 - tbx, sy + 15);
      display.print(names[i]);
    }

    // Color combination badges
    int row2Y = sy + sh + 6;
    int cx = 15;
    uint16_t bgColors[] = {C_RED, C_YELLOW, C_BLACK, C_RED};
    uint16_t fgColors[] = {C_YELLOW, C_RED, C_WHITE, C_BLACK};
    for (int i = 0; i < 4; i++) {
      int x = cx + i * 58;
      display.fillRoundRect(x, row2Y, 48, 22, 4, bgColors[i]);
      display.fillCircle(x + 24, row2Y + 11, 8, fgColors[i]);
    }

    // Full-width color bars at bottom
    int barY = row2Y + 26;
    uint16_t barColors[] = {C_BLACK, C_RED, C_YELLOW, C_WHITE};
    int barW = W / 4;
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
  const uint16_t H = display.height();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    // Header bar
    display.fillRect(0, 0, W, 22, C_RED);
    display.setTextColor(C_WHITE);
    drawCenteredText("Color Typography", 16, &FreeSansBold9pt7b);

    int y = 42;
    display.setFont(&FreeSansBold9pt7b);

    display.setTextColor(C_BLACK);
    display.setCursor(10, y); display.print("Black");
    display.setTextColor(C_RED);
    display.setCursor(80, y); display.print("Red");

    // Yellow text inside black box for contrast
    display.fillRect(140, y - 13, 90, 18, C_BLACK);
    display.setTextColor(C_YELLOW);
    display.setCursor(145, y); display.print("Yellow");

    y += 24;
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(C_RED);
    display.setCursor(10, y); display.print("GDEY0213F52 QuadColor");

    y += 18;
    display.drawFastHLine(10, y, W - 20, C_BLACK);

    y += 6;
    // Color text badge cards
    int bw = 110, bh = 22;
    display.fillRoundRect(10, y, bw, bh, 4, C_RED);
    display.setTextColor(C_WHITE);
    display.setFont(&FreeSans9pt7b);
    display.setCursor(16, y + 15);
    display.print("White on Red");

    display.fillRoundRect(130, y, bw, bh, 4, C_BLACK);
    display.setTextColor(C_YELLOW);
    display.setCursor(134, y + 15);
    display.print("Yel on Black");

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

    // Top Header Bar
    display.fillRect(0, 0, W, 18, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("Color Geometry", 14, &FreeSansBold9pt7b);

    // 1. Cascading Rectangles (left: X = 8..50)
    uint16_t rcColors[] = {C_RED, C_YELLOW, C_BLACK};
    for (int i = 0; i < 3; i++) {
      display.fillRect(8 + i * 12, 26 + i * 10, 42, 26, rcColors[i]);
      display.drawRect(8 + i * 12, 26 + i * 10, 42, 26, C_BLACK);
    }

    // 2. Circles & Triangles (center-left: X = 76..122)
    uint16_t ccColors[] = {C_RED, C_YELLOW, C_BLACK};
    display.fillCircle(88,  36, 10, ccColors[0]); display.drawCircle(88,  36, 10, C_BLACK);
    display.fillCircle(110, 36, 10, ccColors[1]); display.drawCircle(110, 36, 10, C_BLACK);
    display.fillCircle(99,  52, 10, ccColors[2]);

    display.fillTriangle(76, 88, 88, 66, 100, 88, C_RED);
    display.drawTriangle(76, 88, 88, 66, 100, 88, C_BLACK);
    display.fillTriangle(98, 88, 110, 66, 122, 88, C_YELLOW);
    display.drawTriangle(98, 88, 110, 66, 122, 88, C_BLACK);

    // 3. Concentric Circles (center-right: X = 134..186)
    display.drawCircle(160, 56, 26, C_BLACK);
    display.fillCircle(160, 56, 19, C_RED);
    display.fillCircle(160, 56, 12, C_YELLOW);
    display.fillCircle(160, 56, 5,  C_BLACK);

    // 4. 2x2 Color Swatch Grid (right: X = 196..242)
    int gx = 196, gy = 30;
    display.fillRect(gx,      gy,      20, 20, C_RED);
    display.fillRect(gx + 22, gy,      20, 20, C_YELLOW);
    display.fillRect(gx,      gy + 22, 20, 20, C_BLACK);
    display.fillRect(gx + 22, gy + 22, 20, 20, C_WHITE);
    display.drawRect(gx + 22, gy + 22, 20, 20, C_BLACK);
    display.drawRect(gx - 2,  gy - 2,  46, 46, C_BLACK);

    // Bottom Label Footer (Y = 112)
    display.setTextColor(C_BLACK);
    drawCenteredText("GFX Primitives", 112, &FreeSans9pt7b);

  } while (display.nextPage());
}

// =====================================================================
// Screen 5: Color Patterns
// =====================================================================
void showColorPatterns()
{
  display.setRotation(DEMO_ROTATION);
  const uint16_t W = display.width();
  const uint16_t H = display.height();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    display.fillRect(0, 0, W, 22, C_RED);
    display.setTextColor(C_WHITE);
    drawCenteredText("Color Patterns", 16, &FreeSansBold9pt7b);

    int bw = 50, bh = 50;
    uint16_t pColors[] = {C_RED, C_YELLOW, C_BLACK, C_WHITE};

    // 1. Checkerboard
    int bx1 = 10, by1 = 28;
    for (int py = 0; py < bh / 10; py++) {
      for (int px = 0; px < bw / 10; px++) {
        display.fillRect(bx1 + px * 10, by1 + py * 10, 10, 10, pColors[(px + py) % 4]);
      }
    }
    display.drawRect(bx1, by1, bw, bh, C_BLACK);

    // 2. Horizontal stripes
    int bx2 = 70, by2 = 28;
    for (int py = 0; py < bh; py += 10) {
      display.fillRect(bx2, by2 + py, bw, 10, pColors[(py / 10) % 4]);
    }
    display.drawRect(bx2, by2, bw, bh, C_BLACK);

    // 3. Vertical stripes
    int bx3 = 130, by3 = 28;
    for (int px = 0; px < bw; px += 10) {
      display.fillRect(bx3 + px, by3, 10, bh, pColors[(px / 10) % 4]);
    }
    display.drawRect(bx3, by3, bw, bh, C_BLACK);

    // 4. Dot grid
    int bx4 = 190, by4 = 28;
    for (int py = 0; py < bh; py += 12) {
      for (int px = 0; px < bw; px += 12) {
        int ci = (px / 12 + py / 12) % 3;
        display.fillCircle(bx4 + px + 6, by4 + py + 6, 4, pColors[ci]);
      }
    }
    display.drawRect(bx4, by4, bw, bh, C_BLACK);

    // Full-width color bar sequence at bottom
    int barY = 86;
    int barH = 8;
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

  // Header title (standard 5x7 font: setCursor Y is TOP edge)
  display.setTextColor(headerText);
  display.setFont();
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(title, 0, 0, &tbx, &tby, &tbw, &tbh);
  display.setCursor(x + (w - tbw) / 2 - tbx, y + 4);
  display.print(title);

  // Value in FreeSansBold9pt7b (setCursor Y is BASELINE)
  display.setTextColor(accent == C_WHITE ? C_BLACK : (accent == C_YELLOW ? C_BLACK : accent));
  display.setFont(&FreeSansBold9pt7b);
  display.getTextBounds(value, 0, 0, &tbx, &tby, &tbw, &tbh);

  int valX = x + 8 - tbx;
  int valY = y + 31;
  display.setCursor(valX, valY);
  display.print(value);

  // Unit text (standard 5x7 font: setCursor Y is TOP edge)
  display.setTextColor(C_BLACK);
  display.setFont();
  display.setCursor(valX + tbw + 6, y + 23);
  display.print(unit);
}

void showDashboard()
{
  display.setRotation(DEMO_ROTATION); // Landscape 250x122
  const uint16_t W = display.width();
  const uint16_t H = display.height();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    // Header bar (Y 0..16)
    display.fillRect(0, 0, W, 16, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("System Dashboard", 12, &FreeSansBold9pt7b);

    // 2x2 Grid of Cards (Width 116px, Height 38px)
    int cw = 116, ch = 38;
    int col1X = 6, col2X = 128;
    int row1Y = 20, row2Y = 62;

    char uptBuf[16]; snprintf(uptBuf, sizeof(uptBuf), "%lus", millis() / 1000);

    drawColorCard(col1X, row1Y, cw, ch, "TEMPERATURE", "23.5", "degC", C_RED);
    drawColorCard(col2X, row1Y, cw, ch, "HUMIDITY",    "65%",  "RH",   C_BLACK);
    drawColorCard(col1X, row2Y, cw, ch, "BATTERY",     "4.1V", "LiPo", C_YELLOW);
    drawColorCard(col2X, row2Y, cw, ch, "UPTIME",      uptBuf, "running", C_RED);

    // Bottom Status & Progress Bar (Y 105..118)
    int barY = 105;
    display.setFont();
    display.setTextColor(C_BLACK);
    display.setCursor(6, barY + 7);
    display.print("Status:");

    // Colored progress bar segments
    int barX = 52, barW = 150, barH = 11;
    display.drawRect(barX, barY, barW, barH, C_BLACK);
    uint16_t barColors[] = {C_RED, C_YELLOW, C_BLACK, C_RED};
    int segW = barW / 4;
    for (int i = 0; i < 4; i++) {
      display.fillRect(barX + i * segW, barY + 1, segW, barH - 2, barColors[i]);
    }

    display.setTextColor(C_BLACK);
    display.setCursor(barX + barW + 6, barY + 7);
    display.print("100%");

  } while (display.nextPage());
}
