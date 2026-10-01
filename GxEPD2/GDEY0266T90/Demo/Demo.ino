// Demo.ino for GDEY0266T90 on Seeed Studio XIAO MG24 + ePaper Driver Board v2
//
// Comprehensive demo for Seeed Studio XIAO MG24 + ePaper Driver Board v2
//   - 2.66" Monochrome ePaper, 296 x 152
//   - Panel: GDEY0266T90 / Waveshare 2.66inch e-Paper (SSD1680 driver, Black & White)
//   - Driver in GxEPD2: GxEPD2_266_GDEY0266T90
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
// Display: 2.66" Monochrome (152x296) with SSD1680 driver.
// Full-height page buffer: 152x296 needs 1 x 5624 bytes, which the MG24 has
// plenty of RAM for, so firstPage()/nextPage() makes a single pass.
GxEPD2_BW<GxEPD2_266_GDEY0266T90, GxEPD2_266_GDEY0266T90::HEIGHT> display(
  GxEPD2_266_GDEY0266T90(EPD_CS_PIN, EPD_DC_PIN, EPD_RST_PIN, EPD_BUSY_PIN)
);

// Convenience color names for monochrome B/W palette
#define C_BLACK   GxEPD_BLACK
#define C_WHITE   GxEPD_WHITE

// Landscape, 296x152. Rotation 3 rather than GxEPD2's usual 1: this panel's
// origin is the opposite corner on the 24-pin FPC connector. Set to 1 for
// the library-default landscape orientation.
#define DEMO_ROTATION 3

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
  Serial.println(F(" GDEY0266T90 B/W Demo (XIAO MG24 + ePaper Board v2)"));
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

  Serial.println(F("Screen 2: B/W Palette"));
  showColorPalette();
  delay(PAGE_HOLD_MS);

  Serial.println(F("Screen 3: Typography"));
  showColorTypography();
  delay(PAGE_HOLD_MS);

  Serial.println(F("Screen 4: Geometry"));
  showColorGeometry();
  delay(PAGE_HOLD_MS);

  Serial.println(F("Screen 5: Patterns"));
  showColorPatterns();
  delay(PAGE_HOLD_MS);

  Serial.println(F("Screen 6: Dashboard"));
  showDashboard();
  delay(PAGE_HOLD_MS);

  Serial.println(F("Demo complete. Display hibernating."));
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
// Helper: Draw dithered rectangle (50% gray simulation for B/W)
// =====================================================================
void fillDitheredRect(int16_t x, int16_t y, int16_t w, int16_t h)
{
  for (int16_t j = y; j < y + h; j++) {
    for (int16_t i = x; i < x + w; i++) {
      if ((i + j) % 2 == 0) {
        display.drawPixel(i, j, C_BLACK);
      }
    }
  }
}

// =====================================================================
// Screen 1: Splash
// =====================================================================
void showSplashScreen()
{
  display.setRotation(DEMO_ROTATION); // Landscape 296x152
  const uint16_t W = display.width();
  const uint16_t H = display.height();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    // Black top stripe
    display.fillRect(6, 4, W - 12, 6, C_BLACK);

    display.drawRect(4, 14, W - 8, H - 24, C_BLACK);

    display.setTextColor(C_BLACK);
    drawCenteredText("XIAO MG24 + ePaper", 38, &FreeSansBold9pt7b);
    drawCenteredText("Monochrome e-Paper", 62, &FreeSansBold9pt7b);

    display.drawFastHLine(W / 4, 73, W / 2, C_BLACK);

    drawCenteredText("GDEY0266T90 (296x152)", 96, &FreeSans9pt7b);
    drawCenteredText("Seeed Studio ePaper Board v2", 118, &FreeSans9pt7b);

    // Bottom stripe
    display.fillRect(6, H - 8, W - 12, 6, C_BLACK);
  } while (display.nextPage());
}

// =====================================================================
// Screen 2: B/W Palette & Contrast
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
    drawCenteredText("B/W Palette & Contrast", 16, &FreeSansBold9pt7b);

    // 2 Big Swatches: BLACK and WHITE
    const uint16_t swatchColors[] = {C_BLACK, C_WHITE};
    const char* names[] = {"BLACK", "WHITE"};
    int sw = 110, sh = 40, gap = 20;
    int sx = (W - (2 * sw + gap)) / 2;
    int sy = 28;

    for (int i = 0; i < 2; i++) {
      int x = sx + i * (sw + gap);
      display.fillRoundRect(x, sy, sw, sh, 4, swatchColors[i]);
      display.drawRoundRect(x, sy, sw, sh, 4, C_BLACK);
      display.setFont();
      display.setTextColor(swatchColors[i] == C_BLACK ? C_WHITE : C_BLACK);
      int16_t tbx, tby; uint16_t tbw, tbh;
      display.getTextBounds(names[i], 0, 0, &tbx, &tby, &tbw, &tbh);
      display.setCursor(x + (sw - tbw) / 2 - tbx, sy + 16);
      display.print(names[i]);
    }

    // Contrast combination badges
    int row2Y = sy + sh + 8;
    int bw = 110, bgap = 20;
    int cx = (W - (2 * bw + bgap)) / 2;
    uint16_t bgColors[] = {C_BLACK, C_WHITE};
    uint16_t fgColors[] = {C_WHITE, C_BLACK};
    for (int i = 0; i < 2; i++) {
      int x = cx + i * (bw + bgap);
      display.fillRoundRect(x, row2Y, bw, 24, 4, bgColors[i]);
      display.drawRoundRect(x, row2Y, bw, 24, 4, C_BLACK);
      display.fillCircle(x + bw / 2, row2Y + 12, 8, fgColors[i]);
    }

    // Full-width bars at bottom: Solid Black | 50% Dither | Solid White
    int barY = row2Y + 30;
    int barW = (W - 20) / 3;
    display.fillRect(10, barY, barW, H - barY - 4, C_BLACK);
    fillDitheredRect(10 + barW, barY, barW, H - barY - 4);
    display.drawRect(10 + barW, barY, barW, H - barY - 4, C_BLACK);
    display.drawRect(10 + 2 * barW, barY, barW, H - barY - 4, C_BLACK);
  } while (display.nextPage());
}

// =====================================================================
// Screen 3: B/W Typography
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
    display.fillRect(0, 0, W, 22, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("Monochrome Typography", 16, &FreeSansBold9pt7b);

    int y = 46;
    display.setFont(&FreeSansBold9pt7b);
    display.setTextColor(C_BLACK);
    display.setCursor(12, y); display.print("FreeSansBold 9pt");

    // Inverted text sample
    display.fillRect(190, y - 15, 96, 20, C_BLACK);
    display.setTextColor(C_WHITE);
    display.setCursor(196, y); display.print("Inverted");

    y += 24;
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(C_BLACK);
    display.setCursor(12, y); display.print("GDEY0266T90 296x152");

    y += 14;
    display.drawFastHLine(12, y, W - 24, C_BLACK);

    y += 8;
    // Badge cards
    int bw = 130, bh = 24;
    display.fillRoundRect(12, y, bw, bh, 4, C_BLACK);
    display.setTextColor(C_WHITE);
    display.setFont(&FreeSans9pt7b);
    display.setCursor(20, y + 17);
    display.print("White on Black");

    display.drawRoundRect(154, y, bw, bh, 4, C_BLACK);
    display.setTextColor(C_BLACK);
    display.setCursor(162, y + 17);
    display.print("Black on White");

  } while (display.nextPage());
}

// =====================================================================
// Screen 4: B/W Geometry
// =====================================================================
void showColorGeometry()
{
  display.setRotation(DEMO_ROTATION);
  const uint16_t W = display.width();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    // Top Header Bar
    display.fillRect(0, 0, W, 20, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("B/W Geometry & Dithering", 15, &FreeSansBold9pt7b);

    // 1. Cascading Rectangles (left: X = 8..86)
    for (int i = 0; i < 3; i++) {
      if (i == 1) {
        fillDitheredRect(8 + i * 14, 28 + i * 12, 50, 30);
      } else {
        display.fillRect(8 + i * 14, 28 + i * 12, 50, 30, (i == 0) ? C_BLACK : C_WHITE);
      }
      display.drawRect(8 + i * 14, 28 + i * 12, 50, 30, C_BLACK);
    }

    // 2. Circles & Triangles (center-left: X = 98..158)
    display.fillCircle(110, 44, 12, C_BLACK);
    display.drawCircle(140, 44, 12, C_BLACK);
    fillDitheredRect(113, 56, 24, 24);
    display.drawRect(113, 56, 24, 24, C_BLACK);

    display.fillTriangle(98, 116, 112, 88, 126, 116, C_BLACK);
    display.drawTriangle(130, 116, 144, 88, 158, 116, C_BLACK);

    // 3. Concentric Circles (center-right: X = 170..230)
    display.drawCircle(200, 66, 30, C_BLACK);
    display.fillCircle(200, 66, 22, C_BLACK);
    display.fillCircle(200, 66, 14, C_WHITE);
    display.drawCircle(200, 66, 14, C_BLACK);
    display.fillCircle(200, 66, 6,  C_BLACK);

    // 4. 2x2 Swatch Grid (right: X = 246..290)
    int gx = 246, gy = 34;
    display.fillRect(gx,      gy,      20, 20, C_BLACK);
    display.fillRect(gx + 24, gy,      20, 20, C_WHITE);
    fillDitheredRect(gx,      gy + 24, 20, 20);
    display.fillRect(gx + 24, gy + 24, 20, 20, C_BLACK);
    display.drawRect(gx,      gy,      20, 20, C_BLACK);
    display.drawRect(gx + 24, gy,      20, 20, C_BLACK);
    display.drawRect(gx,      gy + 24, 20, 20, C_BLACK);
    display.drawRect(gx + 24, gy + 24, 20, 20, C_BLACK);
    display.drawRect(gx - 3,  gy - 3,  50, 50, C_BLACK);

    // Bottom Label Footer
    display.setTextColor(C_BLACK);
    drawCenteredText("GFX Primitives", 142, &FreeSans9pt7b);

  } while (display.nextPage());
}

// =====================================================================
// Screen 5: B/W Patterns
// =====================================================================
void showColorPatterns()
{
  display.setRotation(DEMO_ROTATION);
  const uint16_t W = display.width();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    display.fillRect(0, 0, W, 22, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("Monochrome Patterns", 16, &FreeSansBold9pt7b);

    int bw = 60, bh = 50;
    int by = 30;

    // 1. Checkerboard
    int bx1 = 12;
    for (int py = 0; py < bh / 10; py++) {
      for (int px = 0; px < bw / 10; px++) {
        if ((px + py) % 2 == 0) {
          display.fillRect(bx1 + px * 10, by + py * 10, 10, 10, C_BLACK);
        }
      }
    }
    display.drawRect(bx1, by, bw, bh, C_BLACK);

    // 2. Horizontal stripes
    int bx2 = 84;
    for (int py = 0; py < bh; py += 8) {
      if ((py / 8) % 2 == 0) {
        display.fillRect(bx2, by + py, bw, 8, C_BLACK);
      }
    }
    display.drawRect(bx2, by, bw, bh, C_BLACK);

    // 3. Vertical stripes
    int bx3 = 156;
    for (int px = 0; px < bw; px += 8) {
      if ((px / 8) % 2 == 0) {
        display.fillRect(bx3 + px, by, 8, bh, C_BLACK);
      }
    }
    display.drawRect(bx3, by, bw, bh, C_BLACK);

    // 4. Dot grid
    int bx4 = 228;
    for (int py = 0; py < bh; py += 14) {
      for (int px = 0; px < bw; px += 14) {
        display.fillCircle(bx4 + px + 7, by + py + 7, 4, C_BLACK);
      }
    }
    display.drawRect(bx4, by, bw, bh, C_BLACK);

    // Dither bars at bottom
    int barY = 96, barH = 14;
    display.fillRect(12, barY, W - 24, barH, C_BLACK);
    fillDitheredRect(12, barY + barH + 4, W - 24, barH);
    display.drawRect(12, barY + barH + 4, W - 24, barH, C_BLACK);

    display.setFont();
    display.setTextColor(C_WHITE);
    display.setCursor(20, barY + 3);
    display.print("100% BLACK");

    display.setTextColor(C_BLACK);
    display.setCursor(20, barY + barH + 7);
    display.print("50% DITHERED GRAY");

  } while (display.nextPage());
}

// =====================================================================
// Screen 6: Dashboard
// =====================================================================
void drawColorCard(int x, int y, int w, int h, const char* title,
                   const char* value, const char* unit, bool inverted)
{
  display.drawRoundRect(x, y, w, h, 4, C_BLACK);

  // Header banner inside card (height 16px)
  display.fillRoundRect(x + 1, y + 1, w - 2, 16, 3, inverted ? C_BLACK : C_WHITE);
  display.drawFastHLine(x + 1, y + 16, w - 2, C_BLACK);

  // Header title
  display.setTextColor(inverted ? C_WHITE : C_BLACK);
  display.setFont();
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(title, 0, 0, &tbx, &tby, &tbw, &tbh);
  display.setCursor(x + (w - tbw) / 2 - tbx, y + 5);
  display.print(title);

  // Value in FreeSansBold9pt7b
  display.setTextColor(C_BLACK);
  display.setFont(&FreeSansBold9pt7b);
  display.getTextBounds(value, 0, 0, &tbx, &tby, &tbw, &tbh);

  int valX = x + 10 - tbx;
  int valY = y + 38;
  display.setCursor(valX, valY);
  display.print(value);

  // Unit text
  display.setTextColor(C_BLACK);
  display.setFont();
  display.setCursor(valX + tbw + 6, y + 30);
  display.print(unit);
}

void showDashboard()
{
  display.setRotation(DEMO_ROTATION); // Landscape 296x152
  const uint16_t W = display.width();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    // Header bar
    display.fillRect(0, 0, W, 18, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("System Dashboard", 13, &FreeSansBold9pt7b);

    // 2x2 Grid of Cards
    int cw = 140, ch = 46;
    int col1X = 6, col2X = 150;
    int row1Y = 24, row2Y = 76;

    char uptBuf[16]; snprintf(uptBuf, sizeof(uptBuf), "%lus", millis() / 1000);

    drawColorCard(col1X, row1Y, cw, ch, "TEMPERATURE", "23.5", "degC", true);
    drawColorCard(col2X, row1Y, cw, ch, "HUMIDITY",    "65%",  "RH",   false);
    drawColorCard(col1X, row2Y, cw, ch, "BATTERY",     "4.1V", "LiPo", false);
    drawColorCard(col2X, row2Y, cw, ch, "UPTIME",      uptBuf, "active", true);

    // Bottom Status & Progress Bar
    int barY = 128;
    display.setFont();
    display.setTextColor(C_BLACK);
    display.setCursor(6, barY + 4);
    display.print("Status:");

    int barX = 58, barW = 174, barH = 14;
    display.drawRect(barX, barY, barW, barH, C_BLACK);
    display.fillRect(barX + 1, barY + 1, barW * 2 / 3, barH - 2, C_BLACK);
    fillDitheredRect(barX + 1 + barW * 2 / 3, barY + 1, barW / 3 - 2, barH - 2);

    display.setTextColor(C_BLACK);
    display.setCursor(barX + barW + 6, barY + 4);
    display.print("OK");

  } while (display.nextPage());
}
