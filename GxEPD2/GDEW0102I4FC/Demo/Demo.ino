// Demo.ino for GDEW0102I4FC on Seeed Studio XIAO MG24 + DESPI-C102
//
// 1.02" Monochrome ePaper, 80 x 128
//   - Panel: Good Display GDEW0102I4FC
//   - Driver IC: UC8175 (Black / White)
//   - Host MCU: XIAO MG24 (Silicon Labs EFR32MG24)
//   - Connection: Good Display DESPI-C102 adapter board (Direct 3.3V)
//
// Pinout:
//   RST  -> D0
//   CS   -> D1
//   BUSY -> D2  (Active LOW on UC8175: 0 = busy, 1 = ready/idle)
//   DC   -> D3
//   SCK  -> D8
//   MOSI -> D10
//   GND  -> GND
//   VDD  -> 3V3

#include <SPI.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

#define EPD_RST_PIN   D0
#define EPD_CS_PIN    D1
#define EPD_BUSY_PIN  D2
#define EPD_DC_PIN    D3
#define EPD_SCK_PIN   D8
#define EPD_MOSI_PIN  D10

// Display constructor: 1.02" monochrome (80x128) GDEW0102I4FC / UC8175
GxEPD2_BW<GxEPD2_102, GxEPD2_102::HEIGHT> display(
  GxEPD2_102(EPD_CS_PIN, EPD_DC_PIN, EPD_RST_PIN, EPD_BUSY_PIN)
);

void showSplashScreen();
void showTypography();
void showGeometry();
void showPartialUpdateDemo();
void showDashboard();

void setup()
{
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  Serial.println();
  Serial.println(F("=================================================="));
  Serial.println(F(" GDEW0102I4FC Demo (Seeed XIAO MG24 + DESPI-C102) "));
  Serial.println(F("=================================================="));

  pinMode(EPD_RST_PIN, OUTPUT);
  pinMode(EPD_DC_PIN,  OUTPUT);
  pinMode(EPD_CS_PIN,  OUTPUT);

  display.init(115200, true, 20, false);

  Serial.print(F("Display dimensions: "));
  Serial.print(display.width());
  Serial.print(F("x"));
  Serial.println(display.height());

  const uint32_t PAGE_HOLD_MS = 3000;

  Serial.println(F("Screen 1: Splash Screen"));
  showSplashScreen();
  delay(PAGE_HOLD_MS);

  Serial.println(F("Screen 2: Typography"));
  showTypography();
  delay(PAGE_HOLD_MS);

  Serial.println(F("Screen 3: Geometry & Patterns"));
  showGeometry();
  delay(PAGE_HOLD_MS);

  Serial.println(F("Screen 4: Partial Update Demo"));
  showPartialUpdateDemo();
  delay(PAGE_HOLD_MS);

  Serial.println(F("Screen 5: Dashboard"));
  showDashboard();
  delay(PAGE_HOLD_MS);

  Serial.println(F("Demo completed. Hibernating display."));
  display.hibernate();
}

void loop()
{
  // Halt execution
}

void showSplashScreen()
{
  display.setRotation(0);
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);

    // Frame border
    display.drawRect(0, 0, display.width(), display.height(), GxEPD_BLACK);
    display.drawRect(2, 2, display.width() - 4, display.height() - 4, GxEPD_BLACK);

    // Header banner
    display.fillRect(4, 4, display.width() - 8, 22, GxEPD_BLACK);
    display.setTextColor(GxEPD_WHITE);
    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(8, 20);
    display.print("1.02\"");

    // Subtitle & Specs
    display.setTextColor(GxEPD_BLACK);
    display.setFont();
    display.setCursor(8, 34);
    display.print("80 x 128");

    display.setCursor(8, 48);
    display.print("UC8175 IC");

    display.setCursor(8, 62);
    display.print("XIAO MG24");

    display.drawFastHLine(6, 76, display.width() - 12, GxEPD_BLACK);

    // Center icon
    int cx = display.width() / 2;
    int cy = 94;
    display.drawRect(cx - 12, cy - 12, 24, 24, GxEPD_BLACK);
    display.fillRect(cx - 6, cy - 6, 12, 12, GxEPD_BLACK);

  } while (display.nextPage());
}

void showTypography()
{
  display.setRotation(0);
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);

    display.fillRect(0, 0, display.width(), 16, GxEPD_BLACK);
    display.setTextColor(GxEPD_WHITE);
    display.setFont();
    display.setCursor(6, 4);
    display.print("TYPOGRAPHY");

    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeSans9pt7b);
    display.setCursor(4, 38);
    display.print("Fonts");

    display.setFont();
    display.setCursor(4, 52);
    display.print("Size 1 Text");

    display.setTextSize(2);
    display.setCursor(4, 68);
    display.print("Size 2");
    display.setTextSize(1);

    display.drawFastHLine(4, 90, display.width() - 8, GxEPD_BLACK);

    display.fillRect(4, 96, display.width() - 8, 24, GxEPD_BLACK);
    display.setTextColor(GxEPD_WHITE);
    display.setCursor(10, 104);
    display.print("INVERTED");

  } while (display.nextPage());
}

void showGeometry()
{
  display.setRotation(0);
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);

    display.fillRect(0, 0, display.width(), 16, GxEPD_BLACK);
    display.setTextColor(GxEPD_WHITE);
    display.setFont();
    display.setCursor(14, 4);
    display.print("GEOMETRY");

    // Geometric shapes
    display.drawRect(8, 24, 30, 24, GxEPD_BLACK);
    display.fillRect(44, 24, 28, 24, GxEPD_BLACK);

    display.drawCircle(23, 68, 14, GxEPD_BLACK);
    display.fillCircle(58, 68, 14, GxEPD_BLACK);

    display.drawTriangle(10, 118, 26, 92, 42, 118, GxEPD_BLACK);
    display.fillTriangle(48, 118, 62, 92, 76, 118, GxEPD_BLACK);

  } while (display.nextPage());
}

void showPartialUpdateDemo()
{
  display.setRotation(0);
  display.setFullWindow();

  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.fillRect(0, 0, display.width(), 16, GxEPD_BLACK);
    display.setTextColor(GxEPD_WHITE);
    display.setFont();
    display.setCursor(10, 4);
    display.print("PARTIAL");

    display.setTextColor(GxEPD_BLACK);
    display.setCursor(6, 26);
    display.print("Fast Update:");

    display.drawRect(15, 42, 50, 40, GxEPD_BLACK);
    display.setCursor(6, 96);
    display.print("Count 1..5");
  } while (display.nextPage());

  uint16_t boxX = 16, boxY = 44, boxW = 48, boxH = 36;
  for (int count = 1; count <= 5; count++) {
    delay(500);
    display.setPartialWindow(boxX, boxY, boxW, boxH);
    display.firstPage();
    do {
      display.fillRect(boxX, boxY, boxW, boxH, GxEPD_WHITE);
      display.setTextColor(GxEPD_BLACK);
      display.setTextSize(3);
      display.setCursor(boxX + 16, boxY + 8);
      display.print(count);
      display.setTextSize(1);
    } while (display.nextPage());
  }
}

void showDashboard()
{
  display.setRotation(0);
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);

    display.fillRect(0, 0, display.width(), 16, GxEPD_BLACK);
    display.setTextColor(GxEPD_WHITE);
    display.setFont();
    display.setCursor(4, 4);
    display.print("STATUS NODE");

    display.setTextColor(GxEPD_BLACK);
    display.setCursor(6, 24);
    display.print("TEMP:");
    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(10, 48);
    display.print("23.5 C");

    display.setFont();
    display.setCursor(6, 60);
    display.print("HUMIDITY:");
    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(10, 84);
    display.print("48.2 %");

    display.setFont();
    display.drawFastHLine(4, 96, display.width() - 8, GxEPD_BLACK);

    display.setCursor(6, 102);
    display.print("BAT: 85%");
    display.drawRect(6, 114, 68, 8, GxEPD_BLACK);
    display.fillRect(8, 116, 56, 4, GxEPD_BLACK);

  } while (display.nextPage());
}

