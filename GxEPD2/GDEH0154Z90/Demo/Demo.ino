// Demo.ino for GDEH0154Z90 on Seeed Studio XIAO MG24 + ePaper Driver Board v2
//
// Comprehensive demo for Seeed Studio XIAO MG24 + ePaper Driver Board v2
//   - 1.54" 3-Color ePaper, 200 x 200
//   - Panel: GDEH0154Z90 (SSD1681 driver, 3-color: Black, White, Red)
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
#include <GxEPD2_3C.h>
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
// Display: 1.54" 3-color (200x200) GDEH0154Z90 with SSD1681 driver
GxEPD2_3C<GxEPD2_154_Z90c, GxEPD2_154_Z90c::HEIGHT> display(
  GxEPD2_154_Z90c(EPD_CS_PIN, EPD_DC_PIN, EPD_RST_PIN, EPD_BUSY_PIN)
);

// Convenience color names for the GDEH0154Z90 3-color palette
#define C_BLACK   GxEPD_BLACK
#define C_WHITE   GxEPD_WHITE
#define C_RED     GxEPD_RED

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
  Serial.println(F(" GDEH0154Z90 3-Color Demo (XIAO MG24 + ePaper Board v2)"));
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
// Screen 1: Splash (200x200)
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

    // Top colorful stripe — 3 native colors
    int stripeH = 8, stripeY = 6;
    uint16_t colors[] = {C_RED, C_BLACK, C_RED};
    int stripeW = (W - 12) / 3;
    for (int i = 0; i < 3; i++) {
      display.fillRect(6 + i * stripeW, stripeY, stripeW, stripeH, colors[i]);
    }

    display.drawRect(6, 18, W - 12, H - 36, C_BLACK);

    display.setTextColor(C_BLACK);
    drawCenteredText("XIAO MG24 + ePaper", 46, &FreeSansBold9pt7b);

    display.setTextColor(C_RED);
    drawCenteredText("Tri-Color e-Paper", 70, &FreeSansBold9pt7b);

    display.drawFastHLine(W / 4, 84, W / 2, C_RED);

    display.setTextColor(C_BLACK);
    drawCenteredText("GDEH0154Z90 (200x200)", 112, &FreeSans9pt7b);

    display.setTextColor(C_BLACK);
    drawCenteredText("SSD1681 Driver", 134, &FreeSans9pt7b);

    display.setTextColor(C_RED);
    drawCenteredText("Seeed ePaper v2", 156, &FreeSans9pt7b);

    // Bottom colorful stripe
    uint16_t bottomColors[] = {C_BLACK, C_RED, C_BLACK};
    for (int i = 0; i < 3; i++) {
      display.fillRect(6 + i * stripeW, H - 14, stripeW, stripeH, bottomColors[i]);
    }
  } while (display.nextPage());
}

// =====================================================================
// Screen 2: Color Palette (200x200)
// =====================================================================
void showColorPalette()
{
  display.setRotation(0);
  const uint16_t W = display.width();
  const uint16_t H = display.height();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    // Header bar
    display.fillRect(0, 0, W, 22, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("3-Color Palette", 16, &FreeSansBold9pt7b);

    // 3 Swatches across width
    const uint16_t swatchColors[] = {C_BLACK, C_WHITE, C_RED};
    const char* names[] = {"BLACK", "WHITE", "RED"};
    int sw = 54, sh = 46, gap = 8;
    int sx = (W - (3 * sw + 2 * gap)) / 2;
    int sy = 30;

    for (int i = 0; i < 3; i++) {
      int x = sx + i * (sw + gap);
      display.fillRoundRect(x, sy, sw, sh, 4, swatchColors[i]);
      display.drawRoundRect(x, sy, sw, sh, 4, C_BLACK);
      display.setFont();
      display.setTextColor(swatchColors[i] == C_BLACK || swatchColors[i] == C_RED ? C_WHITE : C_BLACK);
      int16_t tbx, tby; uint16_t tbw, tbh;
      display.getTextBounds(names[i], 0, 0, &tbx, &tby, &tbw, &tbh);
      display.setCursor(x + (sw - tbw) / 2 - tbx, sy + 20);
      display.print(names[i]);
    }

    // Color combination badges
    int row2Y = sy + sh + 10; // 86
    int cx = 12;
    uint16_t bgColors[] = {C_RED, C_BLACK, C_RED};
    uint16_t fgColors[] = {C_BLACK, C_RED, C_WHITE};
    for (int i = 0; i < 3; i++) {
      int x = cx + i * 60;
      display.fillRoundRect(x, row2Y, 52, 26, 4, bgColors[i]);
      display.fillCircle(x + 26, row2Y + 13, 9, fgColors[i]);
    }

    // Full-width vertical color bars at bottom
    int barY = row2Y + 36; // 122
    uint16_t barColors[] = {C_BLACK, C_RED, C_WHITE};
    int barW = W / 3;
    for (int i = 0; i < 3; i++) {
      display.fillRect(i * barW, barY, barW, H - barY, barColors[i]);
      if (barColors[i] == C_WHITE) {
        display.drawRect(i * barW, barY, barW, H - barY, C_BLACK);
      }
    }
  } while (display.nextPage());
}

// =====================================================================
// Screen 3: Color Typography (200x200)
// =====================================================================
void showColorTypography()
{
  display.setRotation(0);
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

    int y = 46;
    display.setFont(&FreeSansBold9pt7b);

    display.setTextColor(C_BLACK);
    display.setCursor(15, y); display.print("Black");
    display.setTextColor(C_RED);
    display.setCursor(90, y); display.print("Red Text");

    y += 24;
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(C_RED);
    display.setCursor(10, y); display.print("GDEH0154Z90 200x200");

    y += 18;
    display.drawFastHLine(10, y, W - 20, C_BLACK);

    y += 12;
    // Color text badge cards
    int bw = 180, bh = 26;
    display.fillRoundRect(10, y, bw, bh, 4, C_RED);
    display.setTextColor(C_WHITE);
    display.setFont(&FreeSans9pt7b);
    display.setCursor(20, y + 17);
    display.print("White text on Red");

    y += 34;
    display.fillRoundRect(10, y, bw, bh, 4, C_BLACK);
    display.setTextColor(C_RED);
    display.setCursor(20, y + 17);
    display.print("Red text on Black");

    display.setTextColor(C_BLACK);
    drawCenteredText("3-Color GFX Font Demo", H - 8, &FreeSans9pt7b);

  } while (display.nextPage());
}

// =====================================================================
// Screen 4: Color Geometry (200x200)
// =====================================================================
void showColorGeometry()
{
  display.setRotation(0);
  const uint16_t W = display.width();
  const uint16_t H = display.height();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    // Header bar
    display.fillRect(0, 0, W, 20, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("Color Geometry", 14, &FreeSansBold9pt7b);

    // 1. Cascading Rectangles (left: X = 10..70, Y = 28..72)
    uint16_t rcColors[] = {C_RED, C_BLACK, C_WHITE};
    for (int i = 0; i < 3; i++) {
      display.fillRect(10 + i * 12, 28 + i * 10, 42, 26, rcColors[i]);
      display.drawRect(10 + i * 12, 28 + i * 10, 42, 26, C_BLACK);
    }

    // 2. Circles & Triangles (right: X = 110..185)
    display.fillCircle(124, 38, 11, C_RED);   display.drawCircle(124, 38, 11, C_BLACK);
    display.fillCircle(148, 38, 11, C_BLACK);
    display.fillCircle(172, 38, 11, C_RED);   display.drawCircle(172, 38, 11, C_BLACK);

    display.fillTriangle(112, 84, 126, 60, 140, 84, C_RED);
    display.drawTriangle(112, 84, 126, 60, 140, 84, C_BLACK);
    display.fillTriangle(144, 84, 158, 60, 172, 84, C_BLACK);

    // 3. Concentric Circles (bottom left: X = 10..90, Y = 96..146)
    display.drawCircle(50, 120, 24, C_BLACK);
    display.fillCircle(50, 120, 17, C_RED);
    display.fillCircle(50, 120, 10, C_BLACK);
    display.fillCircle(50, 120, 4,  C_WHITE);

    // 4. 3-Color Grid Swatches (bottom right: X = 110..185, Y = 96..146)
    int gx = 118, gy = 98;
    display.fillRect(gx,      gy,      26, 22, C_RED);
    display.fillRect(gx + 28, gy,      26, 22, C_BLACK);
    display.fillRect(gx,      gy + 24, 26, 22, C_BLACK);
    display.fillRect(gx + 28, gy + 24, 26, 22, C_WHITE);
    display.drawRect(gx + 28, gy + 24, 26, 22, C_BLACK);
    display.drawRect(gx - 2,  gy - 2,  58, 50, C_BLACK);

    // Footer label
    display.setTextColor(C_BLACK);
    drawCenteredText("200x200 GFX Shapes", H - 8, &FreeSans9pt7b);

  } while (display.nextPage());
}

// =====================================================================
// Screen 5: Color Patterns (200x200)
// =====================================================================
void showColorPatterns()
{
  display.setRotation(0);
  const uint16_t W = display.width();
  const uint16_t H = display.height();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    display.fillRect(0, 0, W, 22, C_RED);
    display.setTextColor(C_WHITE);
    drawCenteredText("Color Patterns", 16, &FreeSansBold9pt7b);

    int bw = 76, bh = 70;
    uint16_t pColors[] = {C_RED, C_BLACK, C_WHITE};

    // 1. Checkerboard (top left)
    int bx1 = 14, by1 = 28;
    for (int py = 0; py < bh / 10; py++) {
      for (int px = 0; px < bw / 10; px++) {
        display.fillRect(bx1 + px * 10, by1 + py * 10, 10, 10, pColors[(px + py) % 3]);
      }
    }
    display.drawRect(bx1, by1, bw, bh, C_BLACK);

    // 2. Horizontal stripes (top right)
    int bx2 = 110, by2 = 28;
    for (int py = 0; py < bh; py += 10) {
      display.fillRect(bx2, by2 + py, bw, 10, pColors[(py / 10) % 3]);
    }
    display.drawRect(bx2, by2, bw, bh, C_BLACK);

    // 3. Vertical stripes (bottom left)
    int bx3 = 14, by3 = 106;
    for (int px = 0; px < bw; px += 10) {
      display.fillRect(bx3 + px, by3, 10, bh, pColors[(px / 10) % 3]);
    }
    display.drawRect(bx3, by3, bw, bh, C_BLACK);

    // 4. Dot grid (bottom right)
    int bx4 = 110, by4 = 106;
    for (int py = 0; py < bh; py += 14) {
      for (int px = 0; px < bw; px += 14) {
        int ci = (px / 14 + py / 14) % 2;
        display.fillCircle(bx4 + px + 7, by4 + py + 7, 4, ci == 0 ? C_RED : C_BLACK);
      }
    }
    display.drawRect(bx4, by4, bw, bh, C_BLACK);

    // Full-width color bars at bottom
    int barY = 184;
    int barH = 4;
    for (int i = 0; i < 3; i++) {
      display.fillRect(14, barY + i * 5, W - 28, barH, pColors[i]);
      if (pColors[i] == C_WHITE) {
        display.drawRect(14, barY + i * 5, W - 28, barH, C_BLACK);
      }
    }

  } while (display.nextPage());
}

// =====================================================================
// Screen 6: Dashboard (200x200)
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

  // Header title (standard 5x7 font)
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

  int valX = x + (w - tbw) / 2 - tbx;
  int valY = y + 31;
  display.setCursor(valX, valY);
  display.print(value);

  // Unit text below value
  display.setTextColor(C_BLACK);
  display.setFont();
  display.getTextBounds(unit, 0, 0, &tbx, &tby, &tbw, &tbh);
  display.setCursor(x + (w - tbw) / 2 - tbx, y + 40);
  display.print(unit);
}

void showDashboard()
{
  display.setRotation(0);
  const uint16_t W = display.width();
  const uint16_t H = display.height();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    // Header bar (Y 0..18)
    display.fillRect(0, 0, W, 18, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("System Dashboard", 14, &FreeSansBold9pt7b);

    // 2x2 Grid of Cards (Width 88px, Height 50px)
    int cw = 88, ch = 50;
    int col1X = 8, col2X = 104;
    int row1Y = 24, row2Y = 80;

    char uptBuf[16]; snprintf(uptBuf, sizeof(uptBuf), "%lus", millis() / 1000);

    drawColorCard(col1X, row1Y, cw, ch, "TEMP",    "23.5", "degC", C_RED);
    drawColorCard(col2X, row1Y, cw, ch, "HUMID",   "65%",  "RH",   C_BLACK);
    drawColorCard(col1X, row2Y, cw, ch, "BATTERY", "4.1V", "LiPo", C_RED);
    drawColorCard(col2X, row2Y, cw, ch, "UPTIME",  uptBuf, "running", C_BLACK);

    // Status Info Box (Y = 136..165)
    int logY = 136;
    display.drawRoundRect(8, logY, W - 16, 29, 4, C_BLACK);
    display.fillRoundRect(9, logY + 1, W - 18, 12, 3, C_RED);
    display.setTextColor(C_WHITE);
    display.setFont();
    display.setCursor(14, logY + 10);
    display.print("System Status: OK");

    display.setTextColor(C_BLACK);
    display.setCursor(14, logY + 18);
    display.print("GDEH0154Z90 (1.54\" 3-Color)");

    // Bottom Status & Progress Bar (Y = 174..186)
    int barY = 174;
    display.setFont();
    display.setTextColor(C_BLACK);
    display.setCursor(8, barY + 4);
    display.print("Status:");

    // Colored progress bar segments
    int barX = 56, barW = 100, barH = 11;
    display.drawRect(barX, barY, barW, barH, C_BLACK);
    uint16_t barColors[] = {C_RED, C_BLACK, C_RED};
    int segW = barW / 3;
    for (int i = 0; i < 3; i++) {
      display.fillRect(barX + i * segW, barY + 1, segW, barH - 2, barColors[i]);
    }

    display.setTextColor(C_BLACK);
    display.setCursor(barX + barW + 4, barY + 4);
    display.print("100%");

  } while (display.nextPage());
}
