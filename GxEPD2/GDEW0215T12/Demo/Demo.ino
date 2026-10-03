// Demo.ino for GDEW0215T12 on Seeed Studio XIAO MG24 + ePaper Driver Board v2
//
// Demonstration suite for Seeed Studio XIAO MG24 + ePaper Driver Board v2
//   - 2.15" Monochrome ePaper, 208 x 112
//   - Panel: GDEW0215T12 (formerly GDEW0215T11, UC8151D controller, Black & White)
//   - Driver in GxEPD2: GxEPD2_215_GDEW0215T12 (in-sketch panel class)
//   - Host MCU: XIAO MG24 (Silicon Labs EFR32MG24)
//   - Driver Board: Seeed Studio ePaper Driver Board for XIAO v2
//
// Pinout:
//   RST  -> D0
//   CS   -> D1
//   BUSY -> D2 (Active LOW on UC8151D: 0 = busy, 1 = ready)
//   DC   -> D3
//   SCK  -> D8
//   MOSI -> D10

#include <SPI.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

#include "GxEPD2_215_GDEW0215T12.h"

// ===== Pin mapping for Seeed Studio ePaper Driver Board v2 =====
#define EPD_RST_PIN   D0
#define EPD_CS_PIN    D1
#define EPD_BUSY_PIN  D2
#define EPD_DC_PIN    D3
#define EPD_SCK_PIN   D8
#define EPD_MOSI_PIN  D10

// ===== Display Constructor =====
// Full-height buffer: 112x208 fits in a single 2912-byte buffer in RAM.
GxEPD2_BW<GxEPD2_215_GDEW0215T12, GxEPD2_215_GDEW0215T12::HEIGHT> display(
  GxEPD2_215_GDEW0215T12(EPD_CS_PIN, EPD_DC_PIN, EPD_RST_PIN, EPD_BUSY_PIN)
);

#define C_BLACK   GxEPD_BLACK
#define C_WHITE   GxEPD_WHITE

#define DEMO_ROTATION 1 // Landscape 208 x 112

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
  Serial.println(F(" GDEW0215T12 B/W Demo (XIAO MG24 + ePaper Board v2)"));
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
  display.setRotation(DEMO_ROTATION);
  const uint16_t W = display.width();
  const uint16_t H = display.height();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    display.fillRect(4, 3, W - 8, 4, C_BLACK);
    display.drawRect(4, 10, W - 8, H - 18, C_BLACK);

    display.setTextColor(C_BLACK);
    drawCenteredText("XIAO MG24 + ePaper", 28, &FreeSansBold9pt7b);
    drawCenteredText("GDEW0215T12 (2.15\")", 50, &FreeSansBold9pt7b);

    display.drawFastHLine(W / 4, 60, W / 2, C_BLACK);

    display.setFont();
    display.setTextSize(1);
    drawCenteredText("208x112 Monochrome UC8151D", 70, NULL);
    drawCenteredText("Seeed Studio ePaper Board v2", 84, NULL);

    display.fillRect(4, H - 7, W - 8, 4, C_BLACK);
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

    display.setFont(&FreeSansBold9pt7b);
    display.setTextColor(C_BLACK);
    drawCenteredText("Palette Swatches", 16, &FreeSansBold9pt7b);

    int16_t swW = 56;
    int16_t swH = 46;
    int16_t swY = 26;

    // 1. Black swatch
    display.fillRect(12, swY, swW, swH, C_BLACK);
    display.setTextColor(C_WHITE);
    display.setFont();
    display.setCursor(24, swY + 20);
    display.print("BLACK");

    // 2. Dither swatch
    display.drawRect(76, swY, swW, swH, C_BLACK);
    fillDitheredRect(77, swY + 1, swW - 2, swH - 2);
    display.fillRect(86, swY + 16, 36, 14, C_WHITE);
    display.setTextColor(C_BLACK);
    display.setCursor(90, swY + 20);
    display.print("DITHER");

    // 3. White swatch
    display.drawRect(140, swY, swW, swH, C_BLACK);
    display.setCursor(154, swY + 20);
    display.print("WHITE");

    // Bottom info band
    display.fillRect(12, 82, W - 24, 22, C_BLACK);
    display.setTextColor(C_WHITE);
    drawCenteredText("Monochrome 1-Bit Contrast", 97, &FreeSans9pt7b);
  } while (display.nextPage());
}

// =====================================================================
// Screen 3: Typography
// =====================================================================
void showColorTypography()
{
  display.setRotation(DEMO_ROTATION);
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    display.drawRect(2, 2, display.width() - 4, display.height() - 4, C_BLACK);

    display.setTextColor(C_BLACK);
    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(10, 22);
    display.print("FreeSansBold 9pt");

    display.setFont(&FreeSans9pt7b);
    display.setCursor(10, 44);
    display.print("FreeSans 9pt Regular");

    display.setFont();
    display.setTextSize(1);
    display.setCursor(10, 60);
    display.print("Default Font Size 1 (5x7 standard)");

    display.setTextSize(2);
    display.setCursor(10, 76);
    display.print("Size 2 Bold");

    display.setFont();
    display.setTextSize(1);
    display.setCursor(10, 96);
    display.print("Crisp e-Paper Text Rendering");
  } while (display.nextPage());
}

// =====================================================================
// Screen 4: Geometry
// =====================================================================
void showColorGeometry()
{
  display.setRotation(DEMO_ROTATION);
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    display.drawRect(0, 0, display.width(), display.height(), C_BLACK);

    // Concentric circles
    display.drawCircle(32, 40, 26, C_BLACK);
    display.drawCircle(32, 40, 18, C_BLACK);
    display.fillCircle(32, 40, 10, C_BLACK);

    // Rounded rectangle
    display.drawRoundRect(74, 14, 56, 52, 8, C_BLACK);
    fillDitheredRect(80, 20, 44, 40);
    display.fillRect(90, 30, 24, 20, C_WHITE);
    display.drawRect(90, 30, 24, 20, C_BLACK);

    // Triangle
    display.drawTriangle(170, 14, 142, 66, 198, 66, C_BLACK);
    display.fillTriangle(170, 28, 154, 60, 186, 60, C_BLACK);

    // Footer label
    display.setFont(&FreeSans9pt7b);
    display.setTextColor(C_BLACK);
    drawCenteredText("Geometric Primitives", 98, &FreeSans9pt7b);
  } while (display.nextPage());
}

// =====================================================================
// Screen 5: Patterns
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

    // Fine checkerboard pattern
    for (int16_t y = 6; y < 66; y += 4) {
      for (int16_t x = 6; x < 96; x += 4) {
        if (((x / 4) + (y / 4)) % 2 == 0) {
          display.fillRect(x, y, 4, 4, C_BLACK);
        }
      }
    }
    display.drawRect(5, 5, 92, 62, C_BLACK);

    // Horizontal bars
    for (int16_t y = 8; y < 64; y += 8) {
      display.fillRect(106, y, 96, 4, C_BLACK);
    }
    display.drawRect(104, 5, 99, 62, C_BLACK);

    display.setFont(&FreeSansBold9pt7b);
    display.setTextColor(C_BLACK);
    drawCenteredText("Dither & Raster Tests", 94, &FreeSansBold9pt7b);
  } while (display.nextPage());
}

// =====================================================================
// Screen 6: Dashboard
// =====================================================================
void showDashboard()
{
  display.setRotation(DEMO_ROTATION);
  const uint16_t W = display.width();
  const uint16_t H = display.height();
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(C_WHITE);

    // Outer frame
    display.drawRect(0, 0, W, H, C_BLACK);

    // Header bar
    display.fillRect(2, 2, W - 4, 20, C_BLACK);
    display.setTextColor(C_WHITE);
    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(8, 17);
    display.print("NODE-01");
    display.setFont();
    display.setCursor(W - 74, 8);
    display.print("BATT: 98%");

    // Divider
    display.drawFastHLine(2, 64, W - 4, C_BLACK);
    display.drawFastVLine(W / 2, 22, 42, C_BLACK);

    // Left card: Temp
    display.setTextColor(C_BLACK);
    display.setFont();
    display.setCursor(8, 28);
    display.print("TEMPERATURE");
    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(14, 52);
    display.print("23.4 C");

    // Right card: Humidity
    display.setFont();
    display.setCursor(W / 2 + 8, 28);
    display.print("HUMIDITY");
    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(W / 2 + 14, 52);
    display.print("48.2 %");

    // Bottom info band
    display.setFont();
    display.setCursor(8, 72);
    display.print("RF: BLE 5.4 / Matter");
    display.setCursor(8, 86);
    display.print("IP: 2001:db8::14");
    display.setCursor(8, 100);
    display.print("STATUS: ONLINE (3s refresh)");

    // Dithered status badge
    display.drawRect(W - 46, 74, 40, 30, C_BLACK);
    fillDitheredRect(W - 44, 76, 36, 26);
    display.fillRect(W - 38, 83, 24, 12, C_WHITE);
    display.setCursor(W - 34, 86);
    display.print("OK");
  } while (display.nextPage());
}
