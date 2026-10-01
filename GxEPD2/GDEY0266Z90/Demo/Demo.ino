// Demo.ino for GDEY0266Z90 on Seeed Studio XIAO MG24 + ePaper Driver Board v2
//
// Comprehensive demo for Seeed Studio XIAO MG24 + ePaper Driver Board v2
//   - 2.66" 3-Color ePaper, 296 x 152
//   - Panel: GDEY0266Z90 (SSD1680 driver, 3-color: Black, White, Red)
//   - Host MCU: XIAO MG24 (Silicon Labs EFR32MG24)
//   - Driver Board: Seeed Studio ePaper Driver Board for XIAO v2
//
// The Waveshare 2.66inch e-Paper (B) is this panel: GxEPD2 itself aliases them
// in GxEPD2.h as "GDEY0266Z90, Waveshare_2_66_bwr = GDEY0266Z90", and the
// resolution and SSD1680 controller match what the vendor driver in this repo's
// Waveshare_2in66br sketch drives. So no custom panel class is needed here.
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
// Ported from the Adafruit Feather RP2040 ThinkInk version in
// ../../../../AdafruitFeatherThinkInk/: the EPD is on the default SPI here, so
// the SPI1 remap and selectSPI() call that version needed are gone.

#include <SPI.h>
#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

// ===== Pin mapping for Seeed Studio ePaper Driver Board v2 =====
#define EPD_RST_PIN   D0
#define EPD_CS_PIN    D1
#define EPD_BUSY_PIN  D2
#define EPD_DC_PIN    D3
#define EPD_SCK_PIN   D8
#define EPD_MOSI_PIN  D10

// ===== Display Constructor =====
// Display: 2.66" 3-color (152x296) with SSD1680 driver.
// Full-height page buffer: 152x296 needs 2 x 5624 bytes, which the MG24 has
// plenty of, so firstPage()/nextPage() makes a single pass.
GxEPD2_3C<GxEPD2_266c, GxEPD2_266c::HEIGHT> display(
  GxEPD2_266c(EPD_CS_PIN, EPD_DC_PIN, EPD_RST_PIN, EPD_BUSY_PIN)
);

// Convenience color names for the GDEY0266Z90 3-color palette
#define C_BLACK   GxEPD_BLACK
#define C_WHITE   GxEPD_WHITE
#define C_RED     GxEPD_RED

// Landscape, 296x152. Rotation 3 rather than GxEPD2's usual 1: this panel's
// origin is the opposite corner, the same 180 degree offset corrected by
// EPD_2IN66B_ROTATE_180 in Waveshare_2in66br and by setRotation(2) in
// Adafruit_EPD/XIAO_Waveshare_2in66b. It is a panel/FPC fact rather than a board
// one, so it carries over from the ThinkInk connector unchanged. Set to 1 for
// the library-default landscape orientation.
#define DEMO_ROTATION 3

void setup()
{
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println(F("=================================================="));
  Serial.println(F(" GDEY0266Z90 3-Color Demo (XIAO MG24 + ePaper Board v2)"));
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

  // Each full refresh on this panel is ~18-20s, so six screens is already a
  // couple of minutes; keep the holds short.
  const uint32_t PAGE_HOLD_MS = 3000;

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

    // Colorful top stripe — alternating the two ink colors
    int stripeH = 8, stripeY = 4;
    uint16_t colors[] = {C_RED, C_BLACK, C_RED, C_BLACK};
    int stripeW = (W - 10) / 4;
    for (int i = 0; i < 4; i++) {
      display.fillRect(5 + i * stripeW, stripeY, stripeW, stripeH, colors[i]);
    }

    display.drawRect(4, 16, W - 8, H - 30, C_BLACK);

    display.setTextColor(C_BLACK);
    drawCenteredText("XIAO MG24 + ePaper", 44, &FreeSansBold9pt7b);

    display.setTextColor(C_RED);
    drawCenteredText("Tri-Color e-Paper", 68, &FreeSansBold9pt7b);

    display.drawFastHLine(W / 4, 80, W / 2, C_RED);

    display.setTextColor(C_BLACK);
    drawCenteredText("GDEY0266Z90 (296x152)", 100, &FreeSans9pt7b);

    display.setTextColor(C_RED);
    drawCenteredText("Seeed ePaper v2", 122, &FreeSans9pt7b);

    // Bottom colorful stripe
    uint16_t bottomColors[] = {C_BLACK, C_RED, C_BLACK, C_RED};
    for (int i = 0; i < 4; i++) {
      display.fillRect(5 + i * stripeW, H - 12, stripeW, stripeH, bottomColors[i]);
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
    display.fillRect(0, 0, W, 24, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("3-Color Palette", 17, &FreeSansBold9pt7b);

    // 3 Swatches across width
    const uint16_t swatchColors[] = {C_BLACK, C_WHITE, C_RED};
    const char* names[] = {"BLACK", "WHITE", "RED"};
    int sw = 70, sh = 44, gap = 10;
    int sx = (W - (3 * sw + 2 * gap)) / 2;
    int sy = 32;

    for (int i = 0; i < 3; i++) {
      int x = sx + i * (sw + gap);
      display.fillRoundRect(x, sy, sw, sh, 4, swatchColors[i]);
      display.drawRoundRect(x, sy, sw, sh, 4, C_BLACK);
      display.setFont();
      display.setTextColor(swatchColors[i] == C_WHITE ? C_BLACK : C_WHITE);
      int16_t tbx, tby; uint16_t tbw, tbh;
      display.getTextBounds(names[i], 0, 0, &tbx, &tby, &tbw, &tbh);
      display.setCursor(x + (sw - tbw) / 2 - tbx, sy + 18);
      display.print(names[i]);
    }

    // Color combination badges
    int row2Y = sy + sh + 8;
    uint16_t bgColors[] = {C_RED, C_BLACK, C_RED};
    uint16_t fgColors[] = {C_WHITE, C_RED, C_BLACK};
    int bw = 64, bgap = 12;
    int cx = (W - (3 * bw + 2 * bgap)) / 2;
    for (int i = 0; i < 3; i++) {
      int x = cx + i * (bw + bgap);
      display.fillRoundRect(x, row2Y, bw, 26, 4, bgColors[i]);
      display.fillCircle(x + bw / 2, row2Y + 13, 9, fgColors[i]);
    }

    // Full-width color bars at bottom
    int barY = row2Y + 32;
    uint16_t barColors[] = {C_BLACK, C_RED, C_WHITE};
    int barW = W / 3;
    for (int i = 0; i < 3; i++) {
      int w = (i == 2) ? (W - 2 * barW) : barW;
      display.fillRect(i * barW, barY, w, H - barY, barColors[i]);
      if (barColors[i] == C_WHITE) {
        display.drawRect(i * barW, barY, w, H - barY, C_BLACK);
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
    display.fillRect(0, 0, W, 24, C_RED);
    display.setTextColor(C_WHITE);
    drawCenteredText("Color Typography", 17, &FreeSansBold9pt7b);

    int y = 48;
    display.setFont(&FreeSansBold9pt7b);

    display.setTextColor(C_BLACK);
    display.setCursor(12, y); display.print("Black");
    display.setTextColor(C_RED);
    display.setCursor(110, y); display.print("Red");

    // White text needs a dark ground to show at all
    display.fillRect(180, y - 15, 104, 22, C_BLACK);
    display.setTextColor(C_WHITE);
    display.setCursor(188, y); display.print("White");

    y += 26;
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(C_RED);
    display.setCursor(12, y); display.print("GDEY0266Z90 BWR");

    y += 14;
    display.drawFastHLine(12, y, W - 24, C_BLACK);

    y += 8;
    // Color text badge cards
    int bw = 130, bh = 26;
    display.fillRoundRect(12, y, bw, bh, 4, C_RED);
    display.setTextColor(C_WHITE);
    display.setFont(&FreeSans9pt7b);
    display.setCursor(20, y + 18);
    display.print("White on Red");

    display.fillRoundRect(154, y, bw, bh, 4, C_BLACK);
    display.setTextColor(C_RED);
    display.setCursor(162, y + 18);
    display.print("Red on Black");

  } while (display.nextPage());
}

// =====================================================================
// Screen 4: Color Geometry
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
    drawCenteredText("Color Geometry", 15, &FreeSansBold9pt7b);

    // 1. Cascading Rectangles (left: X = 8..86)
    uint16_t rcColors[] = {C_RED, C_BLACK, C_RED};
    for (int i = 0; i < 3; i++) {
      display.fillRect(8 + i * 14, 28 + i * 12, 50, 30, rcColors[i]);
      display.drawRect(8 + i * 14, 28 + i * 12, 50, 30, C_BLACK);
    }

    // 2. Circles & Triangles (center-left: X = 98..158)
    display.fillCircle(110, 44, 12, C_RED);   display.drawCircle(110, 44, 12, C_BLACK);
    display.fillCircle(140, 44, 12, C_WHITE); display.drawCircle(140, 44, 12, C_BLACK);
    display.fillCircle(125, 68, 12, C_BLACK);

    display.fillTriangle(98, 116, 112, 88, 126, 116, C_RED);
    display.drawTriangle(98, 116, 112, 88, 126, 116, C_BLACK);
    display.fillTriangle(130, 116, 144, 88, 158, 116, C_BLACK);

    // 3. Concentric Circles (center-right: X = 170..230)
    display.drawCircle(200, 66, 30, C_BLACK);
    display.fillCircle(200, 66, 22, C_RED);
    display.fillCircle(200, 66, 14, C_WHITE);
    display.drawCircle(200, 66, 14, C_BLACK);
    display.fillCircle(200, 66, 6,  C_BLACK);

    // 4. 2x2 Color Swatch Grid (right: X = 246..290)
    int gx = 246, gy = 34;
    display.fillRect(gx,      gy,      20, 20, C_RED);
    display.fillRect(gx + 24, gy,      20, 20, C_BLACK);
    display.fillRect(gx,      gy + 24, 20, 20, C_BLACK);
    display.fillRect(gx + 24, gy + 24, 20, 20, C_WHITE);
    display.drawRect(gx + 24, gy + 24, 20, 20, C_BLACK);
    display.drawRect(gx - 3,  gy - 3,  50, 50, C_BLACK);

    // Bottom Label Footer
    display.setTextColor(C_BLACK);
    drawCenteredText("GFX Primitives", 142, &FreeSans9pt7b);

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

    display.fillRect(0, 0, W, 24, C_RED);
    display.setTextColor(C_WHITE);
    drawCenteredText("Color Patterns", 17, &FreeSansBold9pt7b);

    int bw = 60, bh = 50;
    int by = 32;
    uint16_t pColors[] = {C_RED, C_BLACK, C_WHITE};

    // 1. Checkerboard
    int bx1 = 12;
    for (int py = 0; py < bh / 10; py++) {
      for (int px = 0; px < bw / 10; px++) {
        display.fillRect(bx1 + px * 10, by + py * 10, 10, 10, pColors[(px + py) % 3]);
      }
    }
    display.drawRect(bx1, by, bw, bh, C_BLACK);

    // 2. Horizontal stripes
    int bx2 = 84;
    for (int py = 0; py < bh; py += 10) {
      display.fillRect(bx2, by + py, bw, 10, pColors[(py / 10) % 3]);
    }
    display.drawRect(bx2, by, bw, bh, C_BLACK);

    // 3. Vertical stripes
    int bx3 = 156;
    for (int px = 0; px < bw; px += 10) {
      display.fillRect(bx3 + px, by, 10, bh, pColors[(px / 10) % 3]);
    }
    display.drawRect(bx3, by, bw, bh, C_BLACK);

    // 4. Dot grid
    int bx4 = 228;
    for (int py = 0; py < bh; py += 14) {
      for (int px = 0; px < bw; px += 14) {
        int ci = (px / 14 + py / 14) % 2; // white dots would be invisible here
        display.fillCircle(bx4 + px + 7, by + py + 7, 5, pColors[ci]);
      }
    }
    display.drawRect(bx4, by, bw, bh, C_BLACK);

    // Full-width color bar sequence at bottom
    int barY = 100, barH = 10;
    for (int i = 0; i < 3; i++) {
      display.fillRect(12, barY + i * (barH + 2), W - 24, barH, pColors[i]);
      if (pColors[i] == C_WHITE) {
        display.drawRect(12, barY + i * (barH + 2), W - 24, barH, C_BLACK);
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

  // Header banner inside card (height 16px)
  uint16_t headerBg = accent;
  uint16_t headerText = (accent == C_WHITE) ? C_BLACK : C_WHITE;

  display.fillRoundRect(x + 1, y + 1, w - 2, 16, 3, headerBg);
  if (accent == C_WHITE) {
    display.drawFastHLine(x + 1, y + 16, w - 2, C_BLACK);
  }

  // Header title (standard 5x7 font: setCursor Y is TOP edge)
  display.setTextColor(headerText);
  display.setFont();
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(title, 0, 0, &tbx, &tby, &tbw, &tbh);
  display.setCursor(x + (w - tbw) / 2 - tbx, y + 5);
  display.print(title);

  // Value in FreeSansBold9pt7b (setCursor Y is BASELINE)
  display.setTextColor(accent == C_WHITE ? C_BLACK : accent);
  display.setFont(&FreeSansBold9pt7b);
  display.getTextBounds(value, 0, 0, &tbx, &tby, &tbw, &tbh);

  int valX = x + 10 - tbx;
  int valY = y + 38;
  display.setCursor(valX, valY);
  display.print(value);

  // Unit text (standard 5x7 font: setCursor Y is TOP edge)
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

    drawColorCard(col1X, row1Y, cw, ch, "TEMPERATURE", "23.5", "degC", C_RED);
    drawColorCard(col2X, row1Y, cw, ch, "HUMIDITY",    "65%",  "RH",   C_BLACK);
    drawColorCard(col1X, row2Y, cw, ch, "BATTERY",     "4.1V", "LiPo", C_BLACK);
    drawColorCard(col2X, row2Y, cw, ch, "UPTIME",      uptBuf, "running", C_RED);

    // Bottom Status & Progress Bar
    int barY = 128;
    display.setFont();
    display.setTextColor(C_BLACK);
    display.setCursor(6, barY + 4);
    display.print("Status:");

    // Colored progress bar segments
    int barX = 58, barW = 174, barH = 14;
    display.drawRect(barX, barY, barW, barH, C_BLACK);
    uint16_t barColors[] = {C_RED, C_BLACK, C_RED};
    int segW = barW / 3;
    for (int i = 0; i < 3; i++) {
      display.fillRect(barX + 1 + i * segW, barY + 1, segW, barH - 2, barColors[i]);
    }

    display.setTextColor(C_BLACK);
    display.setCursor(barX + barW + 6, barY + 4);
    display.print("100%");

  } while (display.nextPage());
}
