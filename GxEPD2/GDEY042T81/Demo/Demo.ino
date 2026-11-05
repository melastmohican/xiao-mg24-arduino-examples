// Demo.ino for GDEY042T81 on Seeed Studio XIAO MG24 + ePaper Driver Board v2
//
// 6-screen monochrome demo for Seeed Studio XIAO MG24 + ePaper Driver Board v2
//   - 4.2" Monochrome ePaper, 400 x 300
//   - Panel: GDEY042T81 / Waveshare 4.2inch e-Paper V2 (SSD1683 controller, Black & White)
//   - Driver in GxEPD2: GxEPD2_420_GDEY042T81
//   - Host MCU: XIAO MG24 (Silicon Labs EFR32MG24)
//   - Driver Board: Seeed Studio ePaper Driver Board for XIAO v2
//
// Pinout (from Seeed ePaper Driver Board v2 wiki):
//   https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
//   RST  -> D0
//   CS   -> D1
//   BUSY -> D2 (Active HIGH on SSD1683: 1 = busy, 0 = idle)
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
// Display: 4.2" Monochrome (400x300) with SSD1683 driver.
// Full-height page buffer: 400x300 needs 15,000 bytes (15 KB), which the MG24's
// 256 KB RAM accommodates in a single page pass.
GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> display(
  GxEPD2_420_GDEY042T81(EPD_CS_PIN, EPD_DC_PIN, EPD_RST_PIN, EPD_BUSY_PIN)
);

// Convenience color names for monochrome B/W palette
#define C_BLACK   GxEPD_BLACK
#define C_WHITE   GxEPD_WHITE

// GxEPD2 rotation 0 is the panel's native 400x300 landscape (used here).
// Rotation 1 would be portrait (300x400); rotations 2/3 flip 180 degrees.
// All screen layouts below are sized for the 400x300 canvas.
#define DEMO_ROTATION 0

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
  Serial.println(F(" GDEY042T81 B/W Demo (XIAO MG24 + ePaper Board v2)"));
  Serial.println(F(" 4.2\" 400x300 Monochrome e-Paper (SSD1683)"));
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

  const uint32_t PAGE_HOLD_MS = 5000;

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
// Helper: Draw text centered inside a horizontal span [x, x+w)
// =====================================================================
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
  display.setRotation(DEMO_ROTATION);
  const uint16_t W = display.width();
  const uint16_t H = display.height();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    display.fillRect(10, 8, W - 20, 6, C_BLACK);
    fillDitheredRect(10, 16, W - 20, 3);

    display.drawRect(8, 22, W - 16, H - 44, C_BLACK);
    display.drawRect(10, 24, W - 20, H - 48, C_BLACK);

    display.setTextColor(C_BLACK);
    drawCenteredText("Seeed Studio XIAO MG24", 52, &FreeSansBold9pt7b);
    drawCenteredText("ePaper Driver Board v2", 76, &FreeSansBold9pt7b);

    display.drawFastHLine(W / 4, 90, W / 2, C_BLACK);
    fillDitheredRect(W / 4, 92, W / 2, 2);

    drawCenteredText("Good Display GDEY042T81", 122, &FreeSans9pt7b);
    drawCenteredText("Waveshare 4.2\" e-Paper V2", 146, &FreeSans9pt7b);
    drawCenteredText("400 x 300 Monochrome", 170, &FreeSans9pt7b);
    drawCenteredText("(SSD1683 Controller)", 194, &FreeSans9pt7b);

    display.drawFastHLine(W / 4, 210, W / 2, C_BLACK);

    drawCenteredText("Fast Refresh Supported", 236, &FreeSans9pt7b);
    drawCenteredText("Partial Update Supported", 260, &FreeSans9pt7b);

    fillDitheredRect(10, H - 16, W - 20, 3);
    display.fillRect(10, H - 11, W - 20, 6, C_BLACK);
  } while (display.nextPage());
}

// =====================================================================
// Screen 2: B/W Palette & Contrast
// =====================================================================
void showColorPalette()
{
  display.setRotation(DEMO_ROTATION);
  const uint16_t W = display.width();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    display.fillRect(0, 0, W, 28, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("B/W Contrast & Palette", 20, &FreeSansBold9pt7b);

    const int sw = 170, sh = 50, gap = 20;
    const int sx = (W - (2 * sw + gap)) / 2;
    const int sy = 38;

    display.fillRoundRect(sx, sy, sw, sh, 6, C_BLACK);
    display.setTextColor(C_WHITE);
    drawTextInSpan("BLACK", sx, sw, sy + sh / 2 + 6, &FreeSansBold9pt7b);

    display.drawRoundRect(sx + sw + gap, sy, sw, sh, 6, C_BLACK);
    display.setTextColor(C_BLACK);
    drawTextInSpan("WHITE", sx + sw + gap, sw, sy + sh / 2 + 6, &FreeSansBold9pt7b);

    const int row2Y = sy + sh + 10;
    const int bh = 40;
    display.fillRoundRect(sx, row2Y, sw, bh, 4, C_BLACK);
    display.fillCircle(sx + sw / 2, row2Y + bh / 2, 13, C_WHITE);

    display.drawRoundRect(sx + sw + gap, row2Y, sw, bh, 4, C_BLACK);
    display.fillCircle(sx + sw + gap + sw / 2, row2Y + bh / 2, 13, C_BLACK);

    const int barY = row2Y + bh + 12;
    const int barW = (W - 40) / 4;
    const int barH = 80;
    const int bx0 = 20;

    display.fillRect(bx0, barY, barW, barH, C_BLACK);

    for (int y = barY; y < barY + barH; y++) {
      for (int x = bx0 + barW; x < bx0 + 2 * barW; x++) {
        if ((x + y) % 3 != 0) display.drawPixel(x, y, C_BLACK);
      }
    }
    display.drawRect(bx0 + barW, barY, barW, barH, C_BLACK);

    fillDitheredRect(bx0 + 2 * barW, barY, barW, barH);
    display.drawRect(bx0 + 2 * barW, barY, barW, barH, C_BLACK);

    display.drawRect(bx0 + 3 * barW, barY, barW, barH, C_BLACK);

    display.setTextColor(C_BLACK);
    const char* labels[4] = {"100%", "66%", "50%", "0%"};
    for (int i = 0; i < 4; i++) {
      drawTextInSpan(labels[i], bx0 + i * barW, barW, barY + barH + 6, nullptr);
    }

    drawCenteredText("Hardware 4-Level Gray Dither", 285, &FreeSans9pt7b);
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

    display.fillRect(0, 0, W, 28, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("Typography Suite", 20, &FreeSansBold9pt7b);

    int y = 50;
    display.setFont(&FreeSansBold9pt7b);
    display.setTextColor(C_BLACK);
    display.setCursor(15, y);
    display.print("FreeSansBold 9pt");

    display.fillRoundRect(W - 105, y - 16, 90, 22, 4, C_BLACK);
    display.setTextColor(C_WHITE);
    drawTextInSpan("INVERT", W - 105, 90, y, &FreeSansBold9pt7b);

    y += 28;
    display.setFont(&FreeSans9pt7b);
    display.setTextColor(C_BLACK);
    display.setCursor(15, y);
    display.print("FreeSans 9pt: clean body text");
    y += 20;
    display.setCursor(15, y);
    display.print("and sensor telemetry data.");

    y += 28;
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(15, y);
    display.print("FreeMonoBold: 400x300");

    y += 24;
    display.setFont(&FreeMono9pt7b);
    display.setCursor(15, y);
    display.print("FreeMono 9pt tabular:");
    y += 20;
    display.setCursor(15, y);
    display.print("HEX: 0x1683 RAM: 15KB");

    y += 14;
    display.drawFastHLine(15, y, W - 30, C_BLACK);

    // Built-in font: cursor y is the TOP of the glyph
    y += 10;
    display.setFont();
    display.setTextSize(1);
    display.setCursor(15, y);
    display.print("Default Font (Size 1): 0123456789");
    y += 14;
    display.setCursor(15, y);
    display.print("The quick brown fox jumps lazy dog");

    y += 20;
    display.setTextSize(2);
    display.setCursor(15, y);
    display.print("Default Size 2");

    display.setTextSize(3);
    display.setCursor(15, y + 26);
    display.print("Size 3");
    display.setTextSize(1);
  } while (display.nextPage());
}

// =====================================================================
// Screen 4: Geometric Primitives
// =====================================================================
void showColorGeometry()
{
  display.setRotation(DEMO_ROTATION);
  const uint16_t W = display.width();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    display.fillRect(0, 0, W, 28, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("Geometric Shapes", 20, &FreeSansBold9pt7b);

    display.drawRect(20, 40, 110, 45, C_BLACK);
    display.fillRect(150, 40, 110, 45, C_BLACK);
    display.drawRoundRect(20, 94, 110, 40, 6, C_BLACK);
    display.fillRoundRect(150, 94, 110, 40, 6, C_BLACK);

    display.drawTriangle(75, 146, 25, 192, 125, 192, C_BLACK);
    display.fillTriangle(205, 146, 155, 192, 255, 192, C_BLACK);

    display.drawFastVLine(280, 40, 152, C_BLACK);

    const int cx = 340, cy = 116;
    for (int r = 8; r <= 40; r += 8) {
      display.drawCircle(cx, cy, r, C_BLACK);
    }
    display.fillCircle(cx, cy, 4, C_BLACK);

    display.drawFastHLine(20, 204, W - 40, C_BLACK);

    const int rx = 20, ry = 214, rw = 150, rh = 70;
    display.drawRoundRect(rx, ry, rw, rh, 6, C_BLACK);
    display.drawLine(rx, ry, rx + rw, ry + rh, C_BLACK);
    display.drawLine(rx, ry + rh, rx + rw, ry, C_BLACK);
    fillDitheredRect(rx + 8, ry + 8, rw - 16, rh - 16);
    display.fillRect(rx + 30, ry + 25, rw - 60, 20, C_WHITE);
    display.drawRect(rx + 30, ry + 25, rw - 60, 20, C_BLACK);
    display.setTextColor(C_BLACK);
    drawTextInSpan("400x300", rx + 30, rw - 60, ry + 31, nullptr);

    display.setFont();
    display.setTextColor(C_BLACK);
    display.setCursor(195, 224);
    display.print("Origin (0,0): top-left");
    display.setCursor(195, 242);
    display.print("Bottom-right: (399,299)");
    display.setCursor(195, 260);
    display.print("Aspect 4:3 Landscape");
    display.setCursor(195, 272);
    display.print("~120 DPI");
  } while (display.nextPage());
}

// =====================================================================
// Screen 5: Patterns & Grids
// =====================================================================
void showColorPatterns()
{
  display.setRotation(DEMO_ROTATION);
  const uint16_t W = display.width();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    display.fillRect(0, 0, W, 28, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("Patterns & Grids", 20, &FreeSansBold9pt7b);

    const int boxX = 17, boxY = 42, boxW = 120, boxH = 100;
    display.drawRect(boxX - 2, boxY - 2, boxW + 4, boxH + 4, C_BLACK);
    const int tileSize = 10;
    for (int y = boxY; y < boxY + boxH; y += tileSize) {
      for (int x = boxX; x < boxX + boxW; x += tileSize) {
        if (((x - boxX) / tileSize + (y - boxY) / tileSize) % 2 == 0) {
          display.fillRect(x, y, tileSize, tileSize, C_BLACK);
        }
      }
    }

    const int hX = 155, hY = 42, hW = 120, hH = 100;
    display.drawRect(hX - 2, hY - 2, hW + 4, hH + 4, C_BLACK);
    for (int offset = -hH; offset < hW; offset += 8) {
      int sx = offset, sy = 0, ex = offset + hH, ey = hH;
      if (sx < 0)  { sy = -sx; sx = 0; }
      if (ex > hW) { ey = hH - (ex - hW); ex = hW; }
      display.drawLine(hX + sx, hY + sy, hX + ex, hY + ey, C_BLACK);
    }

    // Concentric rectangles
    const int cX = 293, cY = 42, cW = 90, cH = 100;
    display.drawRect(cX - 2, cY - 2, cW + 4, cH + 4, C_BLACK);
    for (int d = 0; d < 44; d += 6) {
      display.drawRect(cX + d, cY + d, cW - 2 * d, cH - 2 * d, C_BLACK);
    }

    const int rX = 15, rY = 158, rW = 370, rH = 128;
    display.drawRect(rX, rY, rW, rH, C_BLACK);

    for (int x = rX + 10; x < rX + rW - 10; x += 2) {
      display.drawFastVLine(x, rY + 8, 36, C_BLACK);
    }
    for (int y = rY + 52; y < rY + 82; y += 2) {
      display.drawFastHLine(rX + 10, y, rW - 20, C_BLACK);
    }
    for (int d = 0; d < 16; d += 4) {
      display.drawRect(rX + 100 + d, rY + 90 + d, 170 - 2 * d, 32 - 2 * d, C_BLACK);
    }
  } while (display.nextPage());
}

// =====================================================================
// Screen 6: Dashboard
// =====================================================================
void showDashboard()
{
  display.setRotation(DEMO_ROTATION);
  const uint16_t W = display.width();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    display.fillRect(0, 0, W, 30, C_BLACK);
    display.setTextColor(C_WHITE);
    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(12, 21);
    display.print("XIAO MG24 Sensor Node");

    display.setFont();
    display.setCursor(W - 60, 7);
    display.print("ONLINE");
    display.setCursor(W - 60, 17);
    display.print("4.2\" BW");

    const int cardW = 116, cardH = 70, cardY = 38, cardGap = 10;
    const int cx0 = 15;
    const int cx1 = cx0 + cardW + cardGap;
    const int cx2 = cx1 + cardW + cardGap;

    display.drawRoundRect(cx0, cardY, cardW, cardH, 4, C_BLACK);
    display.fillRect(cx0, cardY, cardW, 18, C_BLACK);
    display.setTextColor(C_WHITE);
    drawTextInSpan("TEMPERATURE", cx0, cardW, cardY + 5, nullptr);
    display.setTextColor(C_BLACK);
    drawTextInSpan("24.8 C", cx0, cardW, cardY + 46, &FreeSansBold9pt7b);
    fillDitheredRect(cx0 + 10, cardY + 54, cardW - 20, 8);
    display.drawRect(cx0 + 10, cardY + 54, cardW - 20, 8, C_BLACK);

    display.drawRoundRect(cx1, cardY, cardW, cardH, 4, C_BLACK);
    display.fillRect(cx1, cardY, cardW, 18, C_BLACK);
    display.setTextColor(C_WHITE);
    drawTextInSpan("HUMIDITY", cx1, cardW, cardY + 5, nullptr);
    display.setTextColor(C_BLACK);
    drawTextInSpan("52.4 %", cx1, cardW, cardY + 46, &FreeSansBold9pt7b);
    display.fillRect(cx1 + 10, cardY + 54, (cardW - 20) * 52 / 100, 8, C_BLACK);
    display.drawRect(cx1 + 10, cardY + 54, cardW - 20, 8, C_BLACK);

    display.drawRoundRect(cx2, cardY, cardW, cardH, 4, C_BLACK);
    display.fillRect(cx2, cardY, cardW, 18, C_BLACK);
    display.setTextColor(C_WHITE);
    drawTextInSpan("PRESSURE", cx2, cardW, cardY + 5, nullptr);
    display.setTextColor(C_BLACK);
    drawTextInSpan("1013 hPa", cx2, cardW, cardY + 46, &FreeSansBold9pt7b);
    display.fillRect(cx2 + 10, cardY + 54, (cardW - 20) * 70 / 100, 8, C_BLACK);
    display.drawRect(cx2 + 10, cardY + 54, cardW - 20, 8, C_BLACK);

    const int bX = 15, bY = 116, bW = W - 30, bH = 44;
    display.drawRoundRect(bX, bY, bW, bH, 4, C_BLACK);
    display.fillRect(bX, bY, bW, 18, C_BLACK);
    display.setTextColor(C_WHITE);
    display.setFont();
    display.setCursor(bX + 10, bY + 5);
    display.print("BATTERY TELEMETRY: 4.12 V (85%)");
    display.fillRect(bX + 10, bY + 24, (bW - 20) * 85 / 100, 14, C_BLACK);
    display.drawRect(bX + 10, bY + 24, bW - 20, 14, C_BLACK);

    const int pY = 168, pH = 124;
    display.drawRoundRect(15, pY, W - 30, pH, 4, C_BLACK);
    display.fillRect(15, pY, W - 30, 22, C_BLACK);
    display.setTextColor(C_WHITE);
    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(25, pY + 16);
    display.print("System Hardware Info");

    display.setTextColor(C_BLACK);
    display.setFont(&FreeSans9pt7b);
    const char* left[4]  = {"MCU: XIAO MG24", "Core: Cortex-M33", "RAM: 256 KB", "Flash: 1.5 MB"};
    const char* right[4] = {"Panel: GDEY042T81", "Driver: SSD1683", "Size: 400x300 BW", "Board: ePaper v2"};
    int ty = pY + 44;
    for (int i = 0; i < 4; i++) {
      display.setCursor(25, ty);
      display.print(left[i]);
      display.setCursor(205, ty);
      display.print(right[i]);
      ty += 22;
    }
  } while (display.nextPage());
}
