// Demo.ino for GDEM0154F51H on Seeed Studio XIAO MG24 + ePaper Driver Board v2
//
// Comprehensive demo for Seeed Studio XIAO MG24 + ePaper Driver Board v2
//   - 1.54" 4-Color ePaper, 200 x 200
//   - Panel: GDEM0154F51H / Waveshare 1.54inch e-Paper (G)
//   - Driver IC: JD79660AA (4-color: Black, White, Red, Yellow)
//   - Host MCU: XIAO MG24 (Silicon Labs EFR32MG24)
//   - Driver Board: Seeed Studio ePaper Driver Board for XIAO v2
//
// Pinout (from Seeed ePaper Driver Board v2 wiki):
//   https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
//   RST  -> D0
//   CS   -> D1
//   BUSY -> D2  (Active LOW on JD79660)
//   DC   -> D3
//   SCK  -> D8
//   MOSI -> D10
//

#include <SPI.h>
#include <GxEPD2_4C.h>
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
// Display: 1.54" 4-color (200x200) GDEM0154F51H with JD79660 driver
GxEPD2_4C<GxEPD2_154c_GDEM0154F51H, GxEPD2_154c_GDEM0154F51H::HEIGHT> display(
  GxEPD2_154c_GDEM0154F51H(EPD_CS_PIN, EPD_DC_PIN, EPD_RST_PIN, EPD_BUSY_PIN)
);

// Convenience color names for the GDEM0154F51H 4-color palette
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
  Serial.println(F(" GDEM0154F51H 4-Color Demo (XIAO MG24 + ePaper v2)"));
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

    // Top colorful stripe — 4 native colors
    int stripeH = 6, stripeY = 6;
    uint16_t colors[] = {C_RED, C_YELLOW, C_BLACK, C_RED};
    int stripeW = (W - 12) / 4;
    for (int i = 0; i < 4; i++) {
      display.fillRect(6 + i * stripeW, stripeY, stripeW, stripeH, colors[i]);
    }

    display.drawRect(6, 16, W - 12, H - 32, C_BLACK);

    display.setTextColor(C_BLACK);
    drawCenteredText("XIAO MG24 + ePaper", 42, &FreeSansBold9pt7b);

    display.setTextColor(C_RED);
    drawCenteredText("Quad-Color e-Paper", 64, &FreeSansBold9pt7b);

    display.drawFastHLine(W / 4, 76, W / 2, C_YELLOW);

    display.setTextColor(C_BLACK);
    drawCenteredText("GDEM0154F51H (200x200)", 100, &FreeSans9pt7b);

    display.setTextColor(C_BLACK);
    drawCenteredText("JD79660 Driver", 122, &FreeSans9pt7b);

    display.setTextColor(C_RED);
    drawCenteredText("Seeed ePaper v2", 144, &FreeSans9pt7b);

    // Bottom colorful stripe
    uint16_t bottomColors[] = {C_BLACK, C_YELLOW, C_RED, C_BLACK};
    for (int i = 0; i < 4; i++) {
      display.fillRect(6 + i * stripeW, H - 12, stripeW, stripeH, bottomColors[i]);
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
    drawCenteredText("4-Color Palette", 16, &FreeSansBold9pt7b);

    // 4 Swatches across width
    const uint16_t swatchColors[] = {C_BLACK, C_WHITE, C_RED, C_YELLOW};
    const char* names[] = {"BLK", "WHT", "RED", "YEL"};
    int sw = 40, sh = 44, gap = 6;
    int sx = (W - (4 * sw + 3 * gap)) / 2;
    int sy = 30;

    for (int i = 0; i < 4; i++) {
      int x = sx + i * (sw + gap);
      display.fillRoundRect(x, sy, sw, sh, 4, swatchColors[i]);
      display.drawRoundRect(x, sy, sw, sh, 4, C_BLACK);
      display.setFont();
      display.setTextColor(swatchColors[i] == C_BLACK || swatchColors[i] == C_RED ? C_WHITE : C_BLACK);
      int16_t tbx, tby; uint16_t tbw, tbh;
      display.getTextBounds(names[i], 0, 0, &tbx, &tby, &tbw, &tbh);
      display.setCursor(x + (sw - tbw) / 2 - tbx, sy + 18);
      display.print(names[i]);
    }

    // Color combination badges
    int row2Y = sy + sh + 8; // 82
    int cx = 10;
    uint16_t bgColors[] = {C_RED, C_YELLOW, C_BLACK, C_RED};
    uint16_t fgColors[] = {C_YELLOW, C_RED, C_WHITE, C_BLACK};
    for (int i = 0; i < 4; i++) {
      int x = cx + i * 46;
      display.fillRoundRect(x, row2Y, 40, 24, 4, bgColors[i]);
      if (bgColors[i] == C_WHITE || bgColors[i] == C_YELLOW) {
        display.drawRoundRect(x, row2Y, 40, 24, 4, C_BLACK);
      }
      display.fillCircle(x + 20, row2Y + 12, 7, fgColors[i]);
      if (fgColors[i] == C_WHITE || fgColors[i] == C_YELLOW) {
        display.drawCircle(x + 20, row2Y + 12, 7, C_BLACK);
      }
    }

    // Full-width vertical color bars at bottom
    int barY = row2Y + 34; // 116
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

    int y = 44;
    display.setFont(&FreeSansBold9pt7b);

    display.setTextColor(C_BLACK);
    display.setCursor(10, y); display.print("Black");
    display.setTextColor(C_RED);
    display.setCursor(75, y); display.print("Red");
    display.setTextColor(C_YELLOW);
    display.setCursor(125, y); display.print("Yellow");

    y += 24;
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(C_BLACK);
    display.setCursor(8, y); display.print("GDEM0154F51H 4C");

    y += 18;
    display.drawFastHLine(8, y, W - 16, C_YELLOW);

    y += 10;
    // Color text badge cards
    int bw = 184, bh = 24;
    display.fillRoundRect(8, y, bw, bh, 4, C_RED);
    display.setTextColor(C_YELLOW);
    display.setFont(&FreeSans9pt7b);
    display.setCursor(18, y + 16);
    display.print("Yellow text on Red");

    y += 30;
    display.fillRoundRect(8, y, bw, bh, 4, C_BLACK);
    display.setTextColor(C_RED);
    display.setCursor(18, y + 16);
    display.print("Red text on Black");

    y += 30;
    display.fillRoundRect(8, y, bw, bh, 4, C_YELLOW);
    display.drawRoundRect(8, y, bw, bh, 4, C_BLACK);
    display.setTextColor(C_BLACK);
    display.setCursor(18, y + 16);
    display.print("Black on Yellow");

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

    // 1. Cascading Rectangles (left: X = 8..64, Y = 28..72)
    uint16_t rcColors[] = {C_RED, C_YELLOW, C_BLACK, C_WHITE};
    for (int i = 0; i < 4; i++) {
      display.fillRect(8 + i * 10, 26 + i * 8, 38, 22, rcColors[i]);
      display.drawRect(8 + i * 10, 26 + i * 8, 38, 22, C_BLACK);
    }

    // 2. Circles & Triangles (right: X = 100..190)
    display.fillCircle(115, 36, 10, C_RED);     display.drawCircle(115, 36, 10, C_BLACK);
    display.fillCircle(142, 36, 10, C_YELLOW);  display.drawCircle(142, 36, 10, C_BLACK);
    display.fillCircle(169, 36, 10, C_BLACK);

    display.fillTriangle(104, 78, 118, 56, 132, 78, C_RED);
    display.drawTriangle(104, 78, 118, 56, 132, 78, C_BLACK);
    display.fillTriangle(136, 78, 150, 56, 164, 78, C_YELLOW);
    display.drawTriangle(136, 78, 150, 56, 164, 78, C_BLACK);
    display.fillTriangle(168, 78, 182, 56, 196, 78, C_BLACK);

    // 3. Concentric Circles (bottom left: X = 8..90, Y = 90..146)
    display.drawCircle(48, 120, 26, C_BLACK);
    display.fillCircle(48, 120, 20, C_RED);
    display.fillCircle(48, 120, 14, C_YELLOW);
    display.fillCircle(48, 120, 8,  C_BLACK);
    display.fillCircle(48, 120, 3,  C_WHITE);

    // 4. 4-Color Grid Swatches (bottom right: X = 105..185, Y = 92..148)
    int gx = 114, gy = 94;
    display.fillRect(gx,      gy,      26, 22, C_RED);
    display.fillRect(gx + 28, gy,      26, 22, C_YELLOW);
    display.drawRect(gx + 28, gy,      26, 22, C_BLACK);
    display.fillRect(gx,      gy + 24, 26, 22, C_BLACK);
    display.fillRect(gx + 28, gy + 24, 26, 22, C_WHITE);
    display.drawRect(gx + 28, gy + 24, 26, 22, C_BLACK);
    display.drawRect(gx - 2,  gy - 2,  58, 50, C_BLACK);

    // Footer label
    display.setTextColor(C_BLACK);
    drawCenteredText("200x200 4C GFX Shapes", H - 8, &FreeSans9pt7b);

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

    int bw = 76, bh = 68;
    uint16_t pColors[] = {C_BLACK, C_WHITE, C_YELLOW, C_RED};

    // 1. 4-color Checkerboard (top left)
    int bx1 = 14, by1 = 28;
    for (int py = 0; py < bh / 10; py++) {
      for (int px = 0; px < bw / 10; px++) {
        display.fillRect(bx1 + px * 10, by1 + py * 10, 10, 10, pColors[(px + py) % 4]);
      }
    }
    display.drawRect(bx1, by1, bw, bh, C_BLACK);

    // 2. Horizontal stripes (top right)
    int bx2 = 110, by2 = 28;
    for (int py = 0; py < bh; py += 10) {
      display.fillRect(bx2, by2 + py, bw, 10, pColors[(py / 10) % 4]);
    }
    display.drawRect(bx2, by2, bw, bh, C_BLACK);

    // 3. Vertical stripes (bottom left)
    int bx3 = 14, by3 = 104;
    for (int px = 0; px < bw; px += 10) {
      display.fillRect(bx3 + px, by3, 10, bh, pColors[(px / 10) % 4]);
    }
    display.drawRect(bx3, by3, bw, bh, C_BLACK);

    // 4. Dot grid (bottom right)
    int bx4 = 110, by4 = 104;
    for (int py = 0; py < bh; py += 14) {
      for (int px = 0; px < bw; px += 14) {
        int ci = (px / 14 + py / 14) % 4;
        display.fillCircle(bx4 + px + 7, by4 + py + 7, 4, pColors[ci]);
        if (pColors[ci] == C_WHITE || pColors[ci] == C_YELLOW) {
          display.drawCircle(bx4 + px + 7, by4 + py + 7, 4, C_BLACK);
        }
      }
    }
    display.drawRect(bx4, by4, bw, bh, C_BLACK);

    // Full-width color bars at bottom
    int barY = 180;
    int barH = 4;
    for (int i = 0; i < 4; i++) {
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
  uint16_t headerText = (accent == C_WHITE || accent == C_YELLOW) ? C_BLACK : C_WHITE;

  display.fillRoundRect(x + 1, y + 1, w - 2, 14, 3, headerBg);
  if (accent == C_WHITE || accent == C_YELLOW) {
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

    // Top status banner
    display.fillRect(0, 0, W, 22, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("SYSTEM STATUS", 16, &FreeSansBold9pt7b);

    // 2x2 Grid of sensor cards (4 native accents)
    int cw = 88, ch = 48;
    int x1 = 8, x2 = 104;
    int y1 = 28, y2 = 82;

    drawColorCard(x1, y1, cw, ch, "TEMP",   "23.5", "deg C", C_RED);
    drawColorCard(x2, y1, cw, ch, "HUMID",  "48%",  "RH",    C_YELLOW);
    drawColorCard(x1, y2, cw, ch, "BATT",   "3.92", "Volts", C_BLACK);
    drawColorCard(x2, y2, cw, ch, "STATUS", "OK",   "READY", C_RED);

    // Bottom section: System Info bar
    int infoY = 136;
    display.fillRoundRect(8, infoY, W - 16, 36, 4, C_WHITE);
    display.drawRoundRect(8, infoY, W - 16, 36, 4, C_BLACK);

    display.setTextColor(C_BLACK);
    display.setFont();
    display.setCursor(16, infoY + 8);
    display.print("Host: XIAO MG24 (Sense)");
    display.setCursor(16, infoY + 18);
    display.print("Panel: GDEM0154F51H 4C");
    display.setCursor(16, infoY + 28);
    display.print("Update: Full (~20s)");

    // Footer bar
    display.fillRect(0, H - 18, W, 18, C_RED);
    display.setTextColor(C_WHITE);
    drawCenteredText("ePaper Driver Board v2", H - 5, &FreeSans9pt7b);

  } while (display.nextPage());
}
