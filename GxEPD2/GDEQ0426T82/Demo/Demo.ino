// Demo.ino
//
// Comprehensive GxEPD2 demo for Seeed Studio XIAO MG24 + ePaper Driver Board v2
//   - Display: 4.26" Black & White (Monochrome) ePaper, 800 x 480
//   - Panel Driver: GxEPD2_426_GDEQ0426T82
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
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
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

// ===== Display: 4.26" Monochrome 800x480 (GDEQ0426T82) =====
#define MAX_DISPLAY_BUFFER_SIZE 65536u
#define MAX_HEIGHT(EPD) \
    (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE) / (EPD::WIDTH / 8) \
         ? EPD::HEIGHT \
         : (MAX_DISPLAY_BUFFER_SIZE) / (EPD::WIDTH / 8))

GxEPD2_BW<GxEPD2_426_GDEQ0426T82, MAX_HEIGHT(GxEPD2_426_GDEQ0426T82)>
    display(GxEPD2_426_GDEQ0426T82(EPD_CS_PIN, EPD_DC_PIN, EPD_RST_PIN, EPD_BUSY_PIN));

#define C_BLACK GxEPD_BLACK
#define C_WHITE GxEPD_WHITE

void drawCenteredText(const char* text, int16_t y, const GFXfont* font);
void fillDitheredRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t density = 2);
void showSplashScreen();
void showGrayscalePalette();
void showTypography();
void showGeometry();
void showPatterns();
void showDashboard();

void setup()
{
  Serial.begin(115200);
  delay(200);
  Serial.println(F("[XIAO MG24] GxEPD2 Demo - 4.26\" BW (GDEQ0426T82)"));

  pinMode(EPD_RST_PIN, OUTPUT);
  pinMode(EPD_DC_PIN,  OUTPUT);
  pinMode(EPD_CS_PIN,  OUTPUT);

  SPI.begin();
  display.init(115200);

  const uint32_t PAGE_HOLD_MS = 5000;

  Serial.println(F("[XIAO MG24] Screen 1: Splash"));
  showSplashScreen();
  delay(PAGE_HOLD_MS);

  Serial.println(F("[XIAO MG24] Screen 2: Grayscale & Dithering"));
  showGrayscalePalette();
  delay(PAGE_HOLD_MS);

  Serial.println(F("[XIAO MG24] Screen 3: Typography"));
  showTypography();
  delay(PAGE_HOLD_MS);

  Serial.println(F("[XIAO MG24] Screen 4: Geometry"));
  showGeometry();
  delay(PAGE_HOLD_MS);

  Serial.println(F("[XIAO MG24] Screen 5: Patterns"));
  showPatterns();
  delay(PAGE_HOLD_MS);

  Serial.println(F("[XIAO MG24] Screen 6: Dashboard"));
  showDashboard();
  delay(PAGE_HOLD_MS);

  Serial.println(F("[XIAO MG24] Demo complete. Hibernating."));
  display.hibernate();
}

void loop() {}

// =====================================================================
void drawCenteredText(const char* text, int16_t y, const GFXfont* font)
{
  display.setFont(font);
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);
  display.setCursor((display.width() - tbw) / 2 - tbx, y);
  display.print(text);
}

void fillDitheredRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t density)
{
  for (int16_t py = y; py < y + h; py++) {
    for (int16_t px = x; px < x + w; px++) {
      if ((px + py) % density == 0) {
        display.drawPixel(px, py, C_BLACK);
      }
    }
  }
}

// =====================================================================
// Screen 1: Splash Screen
// =====================================================================
void showSplashScreen()
{
  const uint16_t W = display.width();
  const uint16_t H = display.height();
  display.setRotation(0);
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    // Decorative top dithered banner
    fillDitheredRect(10, 10, W - 20, 12, 2);

    // Outer border box
    display.drawRect(10, 30, W - 20, H - 40, C_BLACK);
    display.drawRect(12, 32, W - 24, H - 44, C_BLACK);

    // Title banner
    display.fillRect(W / 4, H / 2 - 100, W / 2, 45, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("XIAO MG24 + ePaper", H / 2 - 70, &FreeSansBold24pt7b);

    display.setTextColor(C_BLACK);
    drawCenteredText("4.26\" Monochrome e-Paper Display", H / 2 - 10, &FreeSansBold18pt7b);

    display.drawFastHLine(W / 4, H / 2 + 15, W / 2, C_BLACK);
    display.drawFastHLine(W / 4, H / 2 + 18, W / 2, C_BLACK);

    drawCenteredText("GxEPD2 + GDEQ0426T82 Demo", H / 2 + 50, &FreeSansBold12pt7b);

    drawCenteredText("800 x 480 | Black & White", H / 2 + 85, &FreeSans9pt7b);

    // Bottom dithered banner
    fillDitheredRect(10, H - 22, W - 20, 12, 2);

    drawCenteredText("Seeed Studio ePaper Driver Board v2", H - 35, &FreeSans9pt7b);
  } while (display.nextPage());
}

// =====================================================================
// Screen 2: Grayscale & Dithering / Shades Palette
// =====================================================================
void showGrayscalePalette()
{
  const uint16_t W = display.width();
  const uint16_t H = display.height();
  display.setRotation(0);
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);
    display.fillRect(0, 0, W, 40, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("Monochrome Shades & Dithering", 30, &FreeSansBold12pt7b);

    const char* names[] = {"Solid Black", "50% Dither", "33% Dither", "25% Dither", "Solid White"};
    int sw = 130, sh = 140, gap = 18;
    int sx = (W - 5 * sw - 4 * gap) / 2;
    int sy = 70;

    for (int i = 0; i < 5; i++) {
      int x = sx + i * (sw + gap);
      display.drawRoundRect(x, sy, sw, sh, 6, C_BLACK);
      display.drawRoundRect(x + 1, sy + 1, sw - 2, sh - 2, 5, C_BLACK);

      if (i == 0) {
        display.fillRoundRect(x + 2, sy + 2, sw - 4, sh - 4, 4, C_BLACK);
      } else if (i == 1) {
        fillDitheredRect(x + 2, sy + 2, sw - 4, sh - 4, 2);
      } else if (i == 2) {
        fillDitheredRect(x + 2, sy + 2, sw - 4, sh - 4, 3);
      } else if (i == 3) {
        fillDitheredRect(x + 2, sy + 2, sw - 4, sh - 4, 4);
      } // i == 4 is solid white (empty)

      display.setFont(&FreeSansBold9pt7b);
      display.setTextColor(C_BLACK);
      int16_t tbx, tby; uint16_t tbw, tbh;
      display.getTextBounds(names[i], 0, 0, &tbx, &tby, &tbw, &tbh);
      display.setCursor(x + (sw - tbw) / 2 - tbx, sy + sh + 25);
      display.print(names[i]);
    }

    // High Contrast Cards Row
    int row2Y = sy + sh + 50;
    display.setFont(&FreeSans9pt7b);
    display.setTextColor(C_BLACK);
    display.setCursor(sx, row2Y);
    display.print("Shading & Contrast Combinations:");

    int cx = sx;
    // Card 1: Solid Black with White Text
    display.fillRoundRect(cx, row2Y + 15, 220, 80, 8, C_BLACK);
    display.setTextColor(C_WHITE);
    display.setFont(&FreeSansBold12pt7b);
    display.setCursor(cx + 20, row2Y + 60);
    display.print("Solid Inverted");

    // Card 2: Dithered BG with White Box & Black Text
    cx += 250;
    display.drawRoundRect(cx, row2Y + 15, 220, 80, 8, C_BLACK);
    fillDitheredRect(cx + 2, row2Y + 17, 216, 76, 2);
    display.fillRoundRect(cx + 20, row2Y + 30, 180, 50, 4, C_WHITE);
    display.drawRoundRect(cx + 20, row2Y + 30, 180, 50, 4, C_BLACK);
    display.setTextColor(C_BLACK);
    display.setFont(&FreeSansBold12pt7b);
    display.setCursor(cx + 35, row2Y + 62);
    display.print("Dithered Frame");

    // Card 3: Double Outline Box
    cx += 250;
    display.drawRoundRect(cx, row2Y + 15, 220, 80, 8, C_BLACK);
    display.drawRoundRect(cx + 4, row2Y + 19, 212, 72, 6, C_BLACK);
    display.drawRoundRect(cx + 8, row2Y + 23, 204, 64, 4, C_BLACK);
    display.setTextColor(C_BLACK);
    display.setFont(&FreeSansBold12pt7b);
    display.setCursor(cx + 30, row2Y + 62);
    display.print("Nested Border");

    drawCenteredText("Monochrome GDEQ0426T82 High-Contrast UI", H - 15, &FreeSans9pt7b);
  } while (display.nextPage());
}

// =====================================================================
// Screen 3: Typography & Fonts
// =====================================================================
void showTypography()
{
  const uint16_t W = display.width();
  const uint16_t H = display.height();
  display.setRotation(0);
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);
    display.fillRect(0, 0, W, 40, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("Monochrome Typography", 30, &FreeSansBold12pt7b);

    int y = 82, x = 40;

    display.setTextColor(C_BLACK);
    display.setFont(&FreeSansBold24pt7b);
    display.setCursor(x, y); display.print("Sans Bold 24pt");
    y += 50;

    display.setFont(&FreeSansBold18pt7b);
    display.setCursor(x, y); display.print("Sans Bold 18pt - Sharp & Crisp");
    y += 45;

    display.setFont(&FreeSansBold12pt7b);
    display.setCursor(x, y); display.print("Sans Bold 12pt - High Contrast Reading");
    y += 40;

    display.setFont(&FreeSans9pt7b);
    display.setCursor(x, y); display.print("Sans 9pt - Ideal for body text and metadata paragraphs on 800x480 resolution.");
    y += 35;

    display.drawFastHLine(x, y, W - 80, C_BLACK);
    display.drawFastHLine(x, y + 2, W - 80, C_BLACK);
    y += 25;

    // Badges & Inverted Cards Column 1
    int bx = x, bw = 240, bh = 42, bgap = 16;
    const char* labels[] = {"White on Black", "Black on White", "Dithered Badge", "Double Outlined"};
    for (int i = 0; i < 4; i++) {
      int by2 = y + i * (bh + bgap);
      if (i == 0) {
        display.fillRoundRect(bx, by2, bw, bh, 6, C_BLACK);
        display.setTextColor(C_WHITE);
      } else if (i == 1) {
        display.drawRoundRect(bx, by2, bw, bh, 6, C_BLACK);
        display.drawRoundRect(bx + 1, by2 + 1, bw - 2, bh - 2, 5, C_BLACK);
        display.setTextColor(C_BLACK);
      } else if (i == 2) {
        display.drawRoundRect(bx, by2, bw, bh, 6, C_BLACK);
        fillDitheredRect(bx + 2, by2 + 2, bw - 4, bh - 4, 2);
        display.fillRoundRect(bx + 15, by2 + 6, bw - 30, bh - 12, 4, C_WHITE);
        display.drawRoundRect(bx + 15, by2 + 6, bw - 30, bh - 12, 4, C_BLACK);
        display.setTextColor(C_BLACK);
      } else {
        display.drawRoundRect(bx, by2, bw, bh, 6, C_BLACK);
        display.drawRoundRect(bx + 4, by2 + 4, bw - 8, bh - 8, 4, C_BLACK);
        display.setTextColor(C_BLACK);
      }
      display.setFont(&FreeSansBold12pt7b);
      display.setCursor(bx + 18, by2 + 28);
      display.print(labels[i]);
    }

    // Right Column: Code & Monospace Demo
    int rbx = bx + bw + 40;
    display.fillRoundRect(rbx, y, W - rbx - 40, 218, 6, C_BLACK);
    display.setFont(&FreeMonoBold12pt7b);
    display.setTextColor(C_WHITE);
    display.setCursor(rbx + 15, y + 30);
    display.print("Monospace Code Block");

    display.setFont(&FreeMono9pt7b);
    int ry = y + 60;
    const char* codeLines[] = {
      "#include <GxEPD2_BW.h>",
      "GxEPD2_BW<GxEPD2_426_GDEQ0426T82>",
      "  display(GxEPD2_426_GDEQ0426T82(",
      "    /* CS   */ D1,",
      "    /* DC   */ D3,",
      "    /* RST  */ D0,",
      "    /* BUSY */ D2));"
    };
    for (int i = 0; i < 7; i++) {
      display.setCursor(rbx + 15, ry);
      display.print(codeLines[i]);
      ry += 22;
    }

    display.setTextColor(C_BLACK);
    drawCenteredText("Clean typography on 4.26 inch e-Paper", H - 15, &FreeSans9pt7b);
  } while (display.nextPage());
}

// =====================================================================
// Screen 4: Geometry & GFX Primitives
// =====================================================================
void showGeometry()
{
  const uint16_t W = display.width();
  const uint16_t H = display.height();
  display.setRotation(0);
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);
    display.fillRect(0, 0, W, 40, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("Monochrome GFX Geometry", 30, &FreeSansBold12pt7b);
    display.setTextColor(C_BLACK);

    // Cascading rectangles (filled & dithered)
    for (int i = 0; i < 5; i++) {
      if (i % 2 == 0) {
        display.fillRect(40 + i * 55, 60 + i * 14, 110, 65, C_BLACK);
      } else {
        display.drawRect(40 + i * 55, 60 + i * 14, 110, 65, C_BLACK);
        fillDitheredRect(40 + i * 55 + 2, 60 + i * 14 + 2, 106, 61, 2);
      }
    }

    // Circles (Outline & Filled)
    for (int i = 0; i < 5; i++) {
      if (i % 2 == 0) {
        display.fillCircle(500 + i * 60, 100, 25, C_BLACK);
      } else {
        display.drawCircle(500 + i * 60, 100, 25, C_BLACK);
        display.drawCircle(500 + i * 60, 100, 20, C_BLACK);
      }
    }

    // Triangles
    int ty = 210;
    for (int i = 0; i < 5; i++) {
      int tx2 = 60 + i * 130;
      if (i % 2 == 0) {
        display.fillTriangle(tx2, ty + 60, tx2 + 30, ty, tx2 + 60, ty + 60, C_BLACK);
      } else {
        display.drawTriangle(tx2, ty + 60, tx2 + 30, ty, tx2 + 60, ty + 60, C_BLACK);
        display.drawTriangle(tx2 + 3, ty + 56, tx2 + 30, ty + 6, tx2 + 57, ty + 56, C_BLACK);
      }
    }

    // Nested Concentric Circles
    int oly = 340, ox = 200;
    for (int r = 0; r < 5; r++) {
      for (int t = 0; t < 2; t++) {
        display.drawCircle(ox, oly + 30, 60 - r * 11 + t, C_BLACK);
      }
    }

    // Dithered Swatch Grid (2 rows x 3 cols)
    int wx = 540, wy = 310;
    display.fillRect(wx,        wy,      55, 55, C_BLACK);
    display.drawRect(wx + 55,   wy,      55, 55, C_BLACK);
    fillDitheredRect(wx + 57,   wy + 2,  51, 51, 2);

    display.drawRect(wx + 110,  wy,      55, 55, C_BLACK);
    fillDitheredRect(wx + 112,  wy + 2,  51, 51, 3);

    display.drawRect(wx,        wy + 55, 55, 55, C_BLACK);
    display.drawRect(wx + 2,    wy + 57, 51, 51, C_BLACK);

    display.fillRect(wx + 55,   wy + 55, 55, 55, C_BLACK);
    display.fillCircle(wx + 82, wy + 82, 18, C_WHITE);

    display.drawRect(wx + 110,  wy + 55, 55, 55, C_BLACK);
    display.fillCircle(wx + 137, wy + 82, 18, C_BLACK);

    display.setTextColor(C_BLACK);
    drawCenteredText("High-resolution 800x480 GFX Primitives", H - 15, &FreeSans9pt7b);
  } while (display.nextPage());
}

// =====================================================================
// Screen 5: Patterns & Textures
// =====================================================================
void showPatterns()
{
  const uint16_t W = display.width();
  const uint16_t H = display.height();
  display.setRotation(0);
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);
    display.fillRect(0, 0, W, 40, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("Monochrome Graphic Patterns", 30, &FreeSansBold12pt7b);
    display.setTextColor(C_BLACK);
    display.setFont(&FreeSans9pt7b);

    int bx = 30, by = 55, bw = 150, bh = 150, gap = 25;

    // 1. Checkerboard
    display.setCursor(bx + 10, by + 15);
    display.print("Checkerboard");
    int pby1 = by + 20;
    display.drawRect(bx, pby1, bw, bh, C_BLACK);
    for (int py = 0; py < bh / 15; py++) {
      for (int px = 0; px < bw / 15; px++) {
        if ((px + py) % 2 == 0) {
          display.fillRect(bx + px * 15, pby1 + py * 15, 15, 15, C_BLACK);
        }
      }
    }

    // 2. Horizontal Stripes
    int bx2 = bx + bw + gap; int by2 = by;
    display.setCursor(bx2 + 10, by2 + 15); display.print("H-Stripes");
    int pby2 = by2 + 20;
    display.drawRect(bx2, pby2, bw, bh, C_BLACK);
    for (int py = 0; py < bh; py += 12) {
      if ((py / 12) % 2 == 0) {
        display.fillRect(bx2, pby2 + py, bw, 6, C_BLACK);
      }
    }

    // 3. Vertical Stripes
    int bx3 = bx2 + bw + gap; int by3 = by2;
    display.setCursor(bx3 + 10, by3 + 15); display.print("V-Stripes");
    int pby3 = by3 + 20;
    display.drawRect(bx3, pby3, bw, bh, C_BLACK);
    for (int px = 0; px < bw; px += 12) {
      if ((px / 12) % 2 == 0) {
        display.fillRect(bx3 + px, pby3, 6, bh, C_BLACK);
      }
    }

    // 4. Dot Grid
    int bx4 = bx3 + bw + gap; int by4 = by3;
    display.setCursor(bx4 + 10, by4 + 15); display.print("Dot Matrix");
    int pby4 = by4 + 20;
    display.drawRect(bx4, pby4, bw, bh, C_BLACK);
    for (int py = 0; py < bh; py += 16) {
      for (int px = 0; px < bw; px += 16) {
        display.fillCircle(bx4 + px + 8, pby4 + py + 8, 4, C_BLACK);
      }
    }

    // Full-width dither & hatch bars
    int barY = pby4 + bh + 25;
    display.setCursor(bx, barY - 5);
    display.print("Contrast gradient bars:");

    int barH = 18;
    // Bar 1: Solid Black
    display.fillRect(bx, barY + 10, W - 2 * bx, barH, C_BLACK);
    // Bar 2: 50% Dither
    display.drawRect(bx, barY + 10 + (barH + 4), W - 2 * bx, barH, C_BLACK);
    fillDitheredRect(bx + 1, barY + 11 + (barH + 4), W - 2 * bx - 2, barH - 2, 2);
    // Bar 3: 33% Dither
    display.drawRect(bx, barY + 10 + 2 * (barH + 4), W - 2 * bx, barH, C_BLACK);
    fillDitheredRect(bx + 1, barY + 11 + 2 * (barH + 4), W - 2 * bx - 2, barH - 2, 3);
    // Bar 4: Line Hatch
    int hY = barY + 10 + 3 * (barH + 4);
    display.drawRect(bx, hY, W - 2 * bx, barH, C_BLACK);
    for (int x = bx; x < W - bx; x += 8) {
      display.drawLine(x, hY, x + barH, hY + barH, C_BLACK);
    }

    drawCenteredText("Precision dithering & hatch patterns", H - 15, &FreeSans9pt7b);
  } while (display.nextPage());
}

// =====================================================================
// Screen 6: Dashboard
// =====================================================================
void drawDashboardCard(int x, int y, int w, int h, const char* title,
                       const char* value, const char* unit, bool inverted)
{
  if (inverted) {
    display.fillRoundRect(x, y, w, h, 6, C_BLACK);
    display.fillRoundRect(x + 2, y + 2, w - 4, 26, 4, C_WHITE);
    display.setTextColor(C_BLACK);
    display.setFont(&FreeSansBold12pt7b);
    int16_t tbx, tby; uint16_t tbw, tbh;
    display.getTextBounds(title, 0, 0, &tbx, &tby, &tbw, &tbh);
    display.setCursor(x + (w - tbw) / 2 - tbx, y + 22);
    display.print(title);

    display.setTextColor(C_WHITE);
    display.setFont(&FreeSansBold18pt7b);
    display.getTextBounds(value, 0, 0, &tbx, &tby, &tbw, &tbh);
    display.setCursor(x + (w - tbw) / 2 - tbx, y + h / 2 + 12);
    display.print(value);

    display.setFont(&FreeSans9pt7b);
    display.getTextBounds(unit, 0, 0, &tbx, &tby, &tbw, &tbh);
    display.setCursor(x + (w - tbw) / 2 - tbx, y + h - 10);
    display.print(unit);
  } else {
    display.drawRoundRect(x, y, w, h, 6, C_BLACK);
    display.drawRoundRect(x + 1, y + 1, w - 2, h - 2, 5, C_BLACK);
    display.fillRoundRect(x + 2, y + 2, w - 4, 26, 4, C_BLACK);
    display.setTextColor(C_WHITE);
    display.setFont(&FreeSansBold12pt7b);
    int16_t tbx, tby; uint16_t tbw, tbh;
    display.getTextBounds(title, 0, 0, &tbx, &tby, &tbw, &tbh);
    display.setCursor(x + (w - tbw) / 2 - tbx, y + 22);
    display.print(title);

    display.setTextColor(C_BLACK);
    display.setFont(&FreeSansBold18pt7b);
    display.getTextBounds(value, 0, 0, &tbx, &tby, &tbw, &tbh);
    display.setCursor(x + (w - tbw) / 2 - tbx, y + h / 2 + 12);
    display.print(value);

    display.setFont(&FreeSans9pt7b);
    display.getTextBounds(unit, 0, 0, &tbx, &tby, &tbw, &tbh);
    display.setCursor(x + (w - tbw) / 2 - tbx, y + h - 10);
    display.print(unit);
  }
}

void showDashboard()
{
  const uint16_t W = display.width();
  const uint16_t H = display.height();
  display.setRotation(0);
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);
    display.fillRect(0, 0, W, 40, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("XIAO MG24 System Dashboard", 30, &FreeSansBold12pt7b);
    display.setTextColor(C_BLACK);

    int cw = 170, ch = 130, gap = 20;
    int sx = (W - 4 * cw - 3 * gap) / 2;
    int row1Y = 60;

    char uptBuf[16]; snprintf(uptBuf, sizeof(uptBuf), "%lu", millis() / 1000);
    char heapBuf[16];
#if defined(ESP32) || defined(ESP8266)
    snprintf(heapBuf, sizeof(heapBuf), "%lu", (unsigned long)(ESP.getFreeHeap() / 1024));
#else
    snprintf(heapBuf, sizeof(heapBuf), "%s", "OK");
#endif

    drawDashboardCard(sx,                  row1Y, cw, ch, "Temp",     "24.2",  "Celsius",  false);
    drawDashboardCard(sx + cw + gap,       row1Y, cw, ch, "Humidity", "58",    "% RH",     true);
    drawDashboardCard(sx + 2 * (cw + gap), row1Y, cw, ch, "Heap",    heapBuf, "kB free",  false);
    drawDashboardCard(sx + 3 * (cw + gap), row1Y, cw, ch, "Uptime",  uptBuf,  "seconds",  true);

    // Activity Log Area
    int logY = row1Y + ch + 20;
    display.drawRoundRect(sx, logY, W - 2 * sx, 200, 6, C_BLACK);
    display.drawRoundRect(sx + 1, logY + 1, W - 2 * sx - 2, 198, 5, C_BLACK);
    display.fillRoundRect(sx + 2, logY + 2, W - 2 * sx - 4, 26, 4, C_BLACK);
    display.setTextColor(C_WHITE);
    display.setFont(&FreeSansBold12pt7b);
    display.setCursor(sx + 15, logY + 22);
    display.print("System Status & Diagnostics");
    display.setFont(&FreeMono9pt7b);

    const char* logs[] = {
      " Host MCU: Seeed Studio XIAO MG24",
      " Driver Board: Seeed ePaper Expansion Board v2",
      " Panel: GxEPD2_426_GDEQ0426T82 (4.26\" BW 800x480)",
      " Interface: SPI @ D8(SCK), D10(MOSI), D1(CS)",
      " Status: All 6 demo screens rendered successfully",
    };

    int ly = logY + 52;
    for (int i = 0; i < 5; i++) {
      if (i % 2 == 0) {
        display.fillCircle(sx + 20, ly - 4, 5, C_BLACK);
      } else {
        display.drawCircle(sx + 20, ly - 4, 5, C_BLACK);
      }
      display.setTextColor(C_BLACK);
      display.setCursor(sx + 32, ly);
      display.print(logs[i]);
      ly += 28;
    }

    // Progress Bar
    int barY = logY + 210;
    display.setFont(&FreeSansBold12pt7b);
    display.setTextColor(C_BLACK);
    display.setCursor(sx, barY + 15);
    display.print("Progress:");
    int barX = sx + 170, barW = W - 2 * sx - 210, barH = 20;
    display.drawRect(barX, barY, barW, barH, C_BLACK);
    display.drawRect(barX + 1, barY + 1, barW - 2, barH - 2, C_BLACK);

    // Filled progress segs
    display.fillRect(barX + 2, barY + 2, barW - 4, barH - 4, C_BLACK);

    display.setTextColor(C_BLACK);
    display.setFont(&FreeSans9pt7b);
    display.setCursor(barX + barW + 8, barY + 14);
    display.print("100%");

    drawCenteredText("Ultra-low power monochrome e-Paper display", H - 15, &FreeSans9pt7b);
  } while (display.nextPage());
}
