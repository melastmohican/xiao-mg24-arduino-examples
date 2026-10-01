// Demo for Seeed Studio XIAO MG24 + ePaper Driver Board v2
//
// Display Product: 3.7" 240x416 Monochrome eInk / ePaper Display Panel
//   - Panel / Controller: GDEY037T03, 240 x 416 resolution
//   - Driver in GxEPD2: GxEPD2_370_GDEY037T03
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
GxEPD2_BW<GxEPD2_370_GDEY037T03, GxEPD2_370_GDEY037T03::HEIGHT> display(
  GxEPD2_370_GDEY037T03(EPD_CS_PIN, EPD_DC_PIN, EPD_RST_PIN, EPD_BUSY_PIN)
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
  Serial.println(F(" GDEY037T03 B/W Demo (XIAO MG24 + ePaper Board v2)"));
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
  display.setRotation(1); // Landscape 416x240
  const uint16_t W = display.width();
  const uint16_t H = display.height();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    // Black top stripe
    display.fillRect(10, 8, W - 20, 10, C_BLACK);

    display.drawRect(8, 24, W - 16, H - 40, C_BLACK);

    display.setTextColor(C_BLACK);
    drawCenteredText("XIAO MG24 + ePaper", 60, &FreeSansBold9pt7b);

    drawCenteredText("Monochrome e-Paper", 95, &FreeSansBold9pt7b);

    display.drawFastHLine(W / 4, 115, W / 2, C_BLACK);

    drawCenteredText("GDEY037T03 (416x240)", 145, &FreeSans9pt7b);

    drawCenteredText("GxEPD2_370_GDEY037T03", 175, &FreeSans9pt7b);

    drawCenteredText("Seeed Studio ePaper Driver Board v2", 205, &FreeSans9pt7b);

    // Bottom stripe
    display.fillRect(10, H - 14, W - 20, 10, C_BLACK);
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
    display.fillRect(0, 0, W, 30, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("B/W Palette & Contrast", 20, &FreeSansBold9pt7b);

    // 2 Swatches: BLACK and WHITE
    const uint16_t swatchColors[] = {C_BLACK, C_WHITE};
    const char* names[] = {"BLACK", "WHITE"};
    int sw = 160, sh = 60, gap = 30;
    int sx = (W - (2 * sw + gap)) / 2;
    int sy = 45;

    for (int i = 0; i < 2; i++) {
      int x = sx + i * (sw + gap);
      display.fillRoundRect(x, sy, sw, sh, 6, swatchColors[i]);
      display.drawRoundRect(x, sy, sw, sh, 6, C_BLACK);
      display.setFont();
      display.setTextColor(swatchColors[i] == C_BLACK ? C_WHITE : C_BLACK);
      int16_t tbx, tby; uint16_t tbw, tbh;
      display.getTextBounds(names[i], 0, 0, &tbx, &tby, &tbw, &tbh);
      display.setCursor(x + (sw - tbw) / 2 - tbx, sy + 25);
      display.print(names[i]);
    }

    // Color combination badges
    int row2Y = sy + sh + 15;
    int cx = (W - (2 * 160 + 30)) / 2;
    uint16_t bgColors[] = {C_BLACK, C_WHITE};
    uint16_t fgColors[] = {C_WHITE, C_BLACK};
    for (int i = 0; i < 2; i++) {
      int x = cx + i * 190;
      display.fillRoundRect(x, row2Y, 160, 36, 6, bgColors[i]);
      display.drawRoundRect(x, row2Y, 160, 36, 6, C_BLACK);
      display.fillCircle(x + 80, row2Y + 18, 12, fgColors[i]);
    }

    // Full-width color bars at bottom
    int barY = row2Y + 50;
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
    display.fillRect(0, 0, W, 30, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("Monochrome Typography", 20, &FreeSansBold9pt7b);

    int y = 65;
    display.setFont(&FreeSansBold9pt7b);

    display.setTextColor(C_BLACK);
    display.setCursor(20, y); display.print("Black Text (FreeSansBold)");

    y += 35;
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(C_BLACK);
    display.setCursor(20, y); display.print("GDEY037T03 416x240 Mono");

    y += 30;
    display.drawFastHLine(20, y, W - 40, C_BLACK);

    y += 20;
    // Text badge cards
    int bw = 180, bh = 40;
    display.fillRoundRect(20, y, bw, bh, 6, C_BLACK);
    display.setTextColor(C_WHITE);
    display.setFont(&FreeSans9pt7b);
    display.setCursor(35, y + 25);
    display.print("White on Black");

    display.drawRoundRect(216, y, bw, bh, 6, C_BLACK);
    display.setTextColor(C_BLACK);
    display.setCursor(231, y + 25);
    display.print("Black on White");

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
    display.fillRect(0, 0, W, 26, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("B/W Geometry", 18, &FreeSansBold9pt7b);

    // 1. Cascading Rectangles (left: X = 15..95)
    uint16_t rcColors[] = {C_BLACK, C_WHITE, C_BLACK};
    for (int i = 0; i < 3; i++) {
      display.fillRect(15 + i * 20, 45 + i * 18, 70, 45, rcColors[i]);
      display.drawRect(15 + i * 20, 45 + i * 18, 70, 45, C_BLACK);
    }

    // 2. Circles & Triangles (center-left: X = 135..225)
    display.fillCircle(155, 65, 18, C_BLACK);
    display.drawCircle(195, 65, 18, C_BLACK);
    display.fillCircle(175, 95, 18, C_BLACK);

    display.fillTriangle(135, 160, 155, 120, 175, 160, C_BLACK);
    display.drawTriangle(175, 160, 195, 120, 215, 160, C_BLACK);

    // 3. Concentric Circles (center-right: X = 245..335)
    display.drawCircle(290, 105, 45, C_BLACK);
    display.fillCircle(290, 105, 34, C_BLACK);
    display.fillCircle(290, 105, 22, C_WHITE);
    display.fillCircle(290, 105, 10, C_BLACK);

    // 4. B/W Swatch Grid (right: X = 345..405)
    int gx = 345, gy = 55;
    display.fillRect(gx,      gy,      26, 26, C_BLACK);
    display.fillRect(gx + 30, gy,      26, 26, C_WHITE);
    display.fillRect(gx,      gy + 30, 26, 26, C_WHITE);
    display.fillRect(gx + 30, gy + 30, 26, 26, C_BLACK);
    display.drawRect(gx,      gy,      26, 26, C_BLACK);
    display.drawRect(gx + 30, gy,      26, 26, C_BLACK);
    display.drawRect(gx,      gy + 30, 26, 26, C_BLACK);
    display.drawRect(gx + 30, gy + 30, 26, 26, C_BLACK);
    display.drawRect(gx - 3,  gy - 3,  62, 62, C_BLACK);

    // Bottom Label Footer
    display.setTextColor(C_BLACK);
    drawCenteredText("416x240 GFX Primitives", H - 15, &FreeSans9pt7b);

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

    display.fillRect(0, 0, W, 26, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("B/W Patterns", 18, &FreeSansBold9pt7b);

    int bw = 85, bh = 85;
    uint16_t pColors[] = {C_BLACK, C_WHITE};

    // 1. Checkerboard
    int bx1 = 15, by1 = 40;
    for (int py = 0; py < bh / 15; py++) {
      for (int px = 0; px < bw / 15; px++) {
        display.fillRect(bx1 + px * 15, by1 + py * 15, 15, 15, pColors[(px + py) % 2]);
      }
    }
    display.drawRect(bx1, by1, bw, bh, C_BLACK);

    // 2. Horizontal stripes
    int bx2 = 115, by2 = 40;
    for (int py = 0; py < bh; py += 15) {
      display.fillRect(bx2, by2 + py, bw, 15, pColors[(py / 15) % 2]);
    }
    display.drawRect(bx2, by2, bw, bh, C_BLACK);

    // 3. Vertical stripes
    int bx3 = 215, by3 = 40;
    for (int px = 0; px < bw; px += 15) {
      display.fillRect(bx3 + px, by3, 15, bh, pColors[(px / 15) % 2]);
    }
    display.drawRect(bx3, by3, bw, bh, C_BLACK);

    // 4. Dot grid
    int bx4 = 315, by4 = 40;
    for (int py = 0; py < bh; py += 18) {
      for (int px = 0; px < bw; px += 18) {
        int ci = (px / 18 + py / 18) % 2;
        display.fillCircle(bx4 + px + 9, by4 + py + 9, 6, pColors[ci]);
      }
    }
    display.drawRect(bx4, by4, bw, bh, C_BLACK);

    // Full-width color bar sequence at bottom
    int barY = 145;
    int barH = 20;
    for (int i = 0; i < 2; i++) {
      display.fillRect(15, barY + i * (barH + 8), W - 30, barH, pColors[i]);
      if (pColors[i] == C_WHITE) {
        display.drawRect(15, barY + i * (barH + 8), W - 30, barH, C_BLACK);
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
  display.drawRoundRect(x, y, w, h, 6, C_BLACK);

  // Header banner inside card (height 20px)
  uint16_t headerBg = accent;
  uint16_t headerText = (accent == C_WHITE) ? C_BLACK : C_WHITE;

  display.fillRoundRect(x + 1, y + 1, w - 2, 20, 5, headerBg);
  if (accent == C_WHITE) {
    display.drawFastHLine(x + 1, y + 20, w - 2, C_BLACK);
  }

  // Header title
  display.setTextColor(headerText);
  display.setFont();
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(title, 0, 0, &tbx, &tby, &tbw, &tbh);
  display.setCursor(x + (w - tbw) / 2 - tbx, y + 6);
  display.print(title);

  // Value in FreeSansBold9pt7b
  display.setTextColor(accent == C_WHITE ? C_BLACK : accent);
  display.setFont(&FreeSansBold9pt7b);
  display.getTextBounds(value, 0, 0, &tbx, &tby, &tbw, &tbh);

  int valX = x + 12 - tbx;
  int valY = y + 48;
  display.setCursor(valX, valY);
  display.print(value);

  // Unit text
  display.setTextColor(C_BLACK);
  display.setFont();
  display.setCursor(valX + tbw + 8, y + 40);
  display.print(unit);
}

void showDashboard()
{
  display.setRotation(1); // Landscape 416x240
  const uint16_t W = display.width();
  const uint16_t H = display.height();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    // Header bar (Y 0..26)
    display.fillRect(0, 0, W, 26, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("System Dashboard", 18, &FreeSansBold9pt7b);

    // 2x2 Grid of Cards (Width 190px, Height 68px)
    int cw = 190, ch = 68;
    int col1X = 12, col2X = 214;
    int row1Y = 36, row2Y = 114;

    char uptBuf[16]; snprintf(uptBuf, sizeof(uptBuf), "%lus", millis() / 1000);

    drawColorCard(col1X, row1Y, cw, ch, "TEMPERATURE", "23.5", "degC", C_BLACK);
    drawColorCard(col2X, row1Y, cw, ch, "HUMIDITY",    "65%",  "RH",   C_WHITE);
    drawColorCard(col1X, row2Y, cw, ch, "BATTERY",     "4.1V", "LiPo", C_BLACK);
    drawColorCard(col2X, row2Y, cw, ch, "UPTIME",      uptBuf, "running", C_WHITE);

    // Bottom Status & Progress Bar (Y 195..215)
    int barY = 195;
    display.setFont();
    display.setTextColor(C_BLACK);
    display.setCursor(12, barY + 10);
    display.print("Status:");

    // Progress bar
    int barX = 80, barW = 270, barH = 16;
    display.drawRect(barX, barY, barW, barH, C_BLACK);
    display.fillRect(barX + 2, barY + 2, barW - 4, barH - 4, C_BLACK);

    display.setTextColor(C_BLACK);
    display.setCursor(barX + barW + 10, barY + 10);
    display.print("100%");

  } while (display.nextPage());
}
