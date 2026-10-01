// Demo for 2.13" 250x122 Monochrome ePaper (GDEM0213B74) on Seeed Studio XIAO MG24 + ePaper Driver Board v2
//
// Display Product: 2.13" 250x122 Monochrome ePaper Display Panel (SSD1680 chip)
//   - Panel / Controller: GDEM0213B74 panel with SSD1680 controller
//   - Driver in GxEPD2: GxEPD2_213_B74
//   - Host MCU: XIAO MG24 (Silicon Labs EFR32MG24)
//   - Driver Board: Seeed Studio ePaper Driver Board for XIAO v2
//
// Pinout (from Seeed ePaper Driver Board v2 wiki):
//   https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
//   RST  -> D0
//   CS   -> D1
//   BUSY -> D2
//   DC   -> D3
//   SCK  -> D8
//   MOSI -> D10
//

#include <SPI.h>
#include <GxEPD2_BW.h>
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
GxEPD2_BW<GxEPD2_213_B74, GxEPD2_213_B74::HEIGHT> display(
  GxEPD2_213_B74(EPD_CS_PIN, EPD_DC_PIN, EPD_RST_PIN, EPD_BUSY_PIN)
);

// Convenience color names for monochrome B/W palette
#define C_BLACK   GxEPD_BLACK
#define C_WHITE   GxEPD_WHITE

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
  Serial.println(F(" GDEM0213B74 B/W Demo (XIAO MG24 + ePaper Board v2)"));
  Serial.println(F("=================================================="));

  pinMode(EPD_RST_PIN, OUTPUT);
  pinMode(EPD_DC_PIN,  OUTPUT);
  pinMode(EPD_CS_PIN,  OUTPUT);

  SPI.begin();
  display.init(115200);

  Serial.print(F("Display size: "));
  Serial.print(display.width());
  Serial.print(F("x"));
  Serial.println(display.height());
  Serial.println(F("Display initialized!"));
  Serial.println();

  const uint32_t PAGE_HOLD_MS = 4000;

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
  display.setRotation(1); // Landscape 250x122
  const uint16_t W = display.width();
  const uint16_t H = display.height();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    // Black top stripe
    display.fillRect(5, 4, W - 10, 6, C_BLACK);

    display.drawRect(4, 14, W - 8, H - 22, C_BLACK);

    display.setTextColor(C_BLACK);
    drawCenteredText("XIAO MG24 + ePaper", 34, &FreeSansBold9pt7b);

    drawCenteredText("Monochrome e-Paper", 54, &FreeSansBold9pt7b);

    display.drawFastHLine(W / 4, 63, W / 2, C_BLACK);

    drawCenteredText("GDEM0213B74 (250x122)", 80, &FreeSans9pt7b);

    drawCenteredText("Seeed Studio ePaper Board v2", 98, &FreeSans9pt7b);

    // Bottom stripe
    display.fillRect(5, H - 7, W - 10, 6, C_BLACK);
  } while (display.nextPage());
}

// =====================================================================
// Screen 2: B/W Palette
// =====================================================================
void showColorPalette()
{
  display.setRotation(1);
  const uint16_t W = display.width();
  const uint16_t H = display.height();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    // Header bar
    display.fillRect(0, 0, W, 22, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("B/W Palette & Contrast", 16, &FreeSansBold9pt7b);

    // 2 Swatches: BLACK and WHITE
    const uint16_t swatchColors[] = {C_BLACK, C_WHITE};
    const char* names[] = {"BLACK", "WHITE"};
    int sw = 90, sh = 38, gap = 20;
    int sx = (W - (2 * sw + gap)) / 2;
    int sy = 26;

    for (int i = 0; i < 2; i++) {
      int x = sx + i * (sw + gap);
      display.fillRoundRect(x, sy, sw, sh, 4, swatchColors[i]);
      display.drawRoundRect(x, sy, sw, sh, 4, C_BLACK);
      display.setFont();
      display.setTextColor(swatchColors[i] == C_BLACK ? C_WHITE : C_BLACK);
      int16_t tbx, tby; uint16_t tbw, tbh;
      display.getTextBounds(names[i], 0, 0, &tbx, &tby, &tbw, &tbh);
      display.setCursor(x + (sw - tbw) / 2 - tbx, sy + 15);
      display.print(names[i]);
    }

    // Color combination badges
    int row2Y = sy + sh + 6;
    int cx = (W - (2 * 90 + 20)) / 2;
    uint16_t bgColors[] = {C_BLACK, C_WHITE};
    uint16_t fgColors[] = {C_WHITE, C_BLACK};
    for (int i = 0; i < 2; i++) {
      int x = cx + i * 110;
      display.fillRoundRect(x, row2Y, 90, 22, 4, bgColors[i]);
      display.drawRoundRect(x, row2Y, 90, 22, 4, C_BLACK);
      display.fillCircle(x + 45, row2Y + 11, 8, fgColors[i]);
    }

    // Full-width color bars at bottom
    int barY = row2Y + 26;
    uint16_t barColors[] = {C_BLACK, C_WHITE};
    int barW = W / 2;
    for (int i = 0; i < 2; i++) {
      display.fillRect(i * barW, barY, barW, H - barY, barColors[i]);
      if (barColors[i] == C_WHITE) {
        display.drawRect(i * barW, barY, barW, H - barY, C_BLACK);
      }
    }
  } while (display.nextPage());
}

// =====================================================================
// Screen 3: B/W Typography
// =====================================================================
void showColorTypography()
{
  display.setRotation(1);
  const uint16_t W = display.width();
  const uint16_t H = display.height();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    // Header bar
    display.fillRect(0, 0, W, 22, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("Monochrome Typography", 16, &FreeSansBold9pt7b);

    int y = 42;
    display.setFont(&FreeSansBold9pt7b);

    display.setTextColor(C_BLACK);
    display.setCursor(20, y); display.print("Black Text");

    y += 24;
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(C_BLACK);
    display.setCursor(10, y); display.print("GDEM0213B74 B/W 122x250");

    y += 18;
    display.drawFastHLine(10, y, W - 20, C_BLACK);

    y += 6;
    // Color text badge cards
    int bw = 110, bh = 22;
    display.fillRoundRect(10, y, bw, bh, 4, C_BLACK);
    display.setTextColor(C_WHITE);
    display.setFont(&FreeSans9pt7b);
    display.setCursor(16, y + 15);
    display.print("White on Blk");

    display.drawRoundRect(130, y, bw, bh, 4, C_BLACK);
    display.setTextColor(C_BLACK);
    display.setCursor(134, y + 15);
    display.print("Blk on White");

  } while (display.nextPage());
}

// =====================================================================
// Screen 4: B/W Geometry
// =====================================================================
void showColorGeometry()
{
  display.setRotation(1);
  const uint16_t W = display.width();
  const uint16_t H = display.height();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    // Top Header Bar
    display.fillRect(0, 0, W, 18, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("B/W Geometry", 14, &FreeSansBold9pt7b);

    // 1. Cascading Rectangles (left: X = 8..50)
    uint16_t rcColors[] = {C_BLACK, C_WHITE, C_BLACK};
    for (int i = 0; i < 3; i++) {
      display.fillRect(8 + i * 12, 26 + i * 10, 42, 26, rcColors[i]);
      display.drawRect(8 + i * 12, 26 + i * 10, 42, 26, C_BLACK);
    }

    // 2. Circles & Triangles (center-left: X = 76..122)
    display.fillCircle(88,  36, 10, C_BLACK);
    display.drawCircle(110, 36, 10, C_BLACK);
    display.fillCircle(99,  52, 10, C_BLACK);

    display.fillTriangle(76, 88, 88, 66, 100, 88, C_BLACK);
    display.drawTriangle(98, 88, 110, 66, 122, 88, C_BLACK);

    // 3. Concentric Circles (center-right: X = 134..186)
    display.drawCircle(160, 56, 26, C_BLACK);
    display.fillCircle(160, 56, 19, C_BLACK);
    display.fillCircle(160, 56, 12, C_WHITE);
    display.fillCircle(160, 56, 5,  C_BLACK);

    // 4. B/W Swatch Grid (right: X = 196..242)
    int gx = 196, gy = 30;
    display.fillRect(gx,      gy,      20, 20, C_BLACK);
    display.fillRect(gx + 22, gy,      20, 20, C_WHITE);
    display.fillRect(gx,      gy + 22, 20, 20, C_WHITE);
    display.fillRect(gx + 22, gy + 22, 20, 20, C_BLACK);
    display.drawRect(gx,      gy,      20, 20, C_BLACK);
    display.drawRect(gx + 22, gy,      20, 20, C_BLACK);
    display.drawRect(gx,      gy + 22, 20, 20, C_BLACK);
    display.drawRect(gx + 22, gy + 22, 20, 20, C_BLACK);
    display.drawRect(gx - 2,  gy - 2,  46, 46, C_BLACK);

    // Bottom Label Footer (Y = 112)
    display.setTextColor(C_BLACK);
    drawCenteredText("GFX Primitives", 112, &FreeSans9pt7b);

  } while (display.nextPage());
}

// =====================================================================
// Screen 5: B/W Patterns
// =====================================================================
void showColorPatterns()
{
  display.setRotation(1);
  const uint16_t W = display.width();
  const uint16_t H = display.height();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    display.fillRect(0, 0, W, 22, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("B/W Patterns", 16, &FreeSansBold9pt7b);

    int bw = 50, bh = 50;
    uint16_t pColors[] = {C_BLACK, C_WHITE};

    // 1. Checkerboard
    int bx1 = 10, by1 = 28;
    for (int py = 0; py < bh / 10; py++) {
      for (int px = 0; px < bw / 10; px++) {
        display.fillRect(bx1 + px * 10, by1 + py * 10, 10, 10, pColors[(px + py) % 2]);
      }
    }
    display.drawRect(bx1, by1, bw, bh, C_BLACK);

    // 2. Horizontal stripes
    int bx2 = 70, by2 = 28;
    for (int py = 0; py < bh; py += 10) {
      display.fillRect(bx2, by2 + py, bw, 10, pColors[(py / 10) % 2]);
    }
    display.drawRect(bx2, by2, bw, bh, C_BLACK);

    // 3. Vertical stripes
    int bx3 = 130, by3 = 28;
    for (int px = 0; px < bw; px += 10) {
      display.fillRect(bx3 + px, by3, 10, bh, pColors[(px / 10) % 2]);
    }
    display.drawRect(bx3, by3, bw, bh, C_BLACK);

    // 4. Dot grid
    int bx4 = 190, by4 = 28;
    for (int py = 0; py < bh; py += 12) {
      for (int px = 0; px < bw; px += 12) {
        int ci = (px / 12 + py / 12) % 2;
        display.fillCircle(bx4 + px + 6, by4 + py + 6, 4, pColors[ci]);
      }
    }
    display.drawRect(bx4, by4, bw, bh, C_BLACK);

    // Full-width color bar sequence at bottom
    int barY = 86;
    int barH = 12;
    for (int i = 0; i < 2; i++) {
      display.fillRect(10, barY + i * (barH + 4), W - 20, barH, pColors[i]);
      if (pColors[i] == C_WHITE) {
        display.drawRect(10, barY + i * (barH + 4), W - 20, barH, C_BLACK);
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
  uint16_t headerText = (accent == C_WHITE) ? C_BLACK : C_WHITE;

  display.fillRoundRect(x + 1, y + 1, w - 2, 14, 3, headerBg);
  if (accent == C_WHITE) {
    display.drawFastHLine(x + 1, y + 14, w - 2, C_BLACK);
  }

  // Header title
  display.setTextColor(headerText);
  display.setFont();
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(title, 0, 0, &tbx, &tby, &tbw, &tbh);
  display.setCursor(x + (w - tbw) / 2 - tbx, y + 4);
  display.print(title);

  // Value in FreeSansBold9pt7b
  display.setTextColor(accent == C_WHITE ? C_BLACK : accent);
  display.setFont(&FreeSansBold9pt7b);
  display.getTextBounds(value, 0, 0, &tbx, &tby, &tbw, &tbh);

  int valX = x + 8 - tbx;
  int valY = y + 31;
  display.setCursor(valX, valY);
  display.print(value);

  // Unit text
  display.setTextColor(C_BLACK);
  display.setFont();
  display.setCursor(valX + tbw + 6, y + 23);
  display.print(unit);
}

void showDashboard()
{
  display.setRotation(1); // Landscape 250x122
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

    drawColorCard(col1X, row1Y, cw, ch, "TEMPERATURE", "23.5", "degC", C_BLACK);
    drawColorCard(col2X, row1Y, cw, ch, "HUMIDITY",    "65%",  "RH",   C_WHITE);
    drawColorCard(col1X, row2Y, cw, ch, "BATTERY",     "4.1V", "LiPo", C_BLACK);
    drawColorCard(col2X, row2Y, cw, ch, "UPTIME",      uptBuf, "running", C_WHITE);

    // Bottom Status & Progress Bar (Y 105..118)
    int barY = 105;
    display.setFont();
    display.setTextColor(C_BLACK);
    display.setCursor(6, barY + 7);
    display.print("Status:");

    // Colored progress bar segments
    int barX = 52, barW = 150, barH = 11;
    display.drawRect(barX, barY, barW, barH, C_BLACK);
    display.fillRect(barX + 1, barY + 1, barW - 2, barH - 2, C_BLACK);

    display.setTextColor(C_BLACK);
    display.setCursor(barX + barW + 6, barY + 7);
    display.print("100%");

  } while (display.nextPage());
}
