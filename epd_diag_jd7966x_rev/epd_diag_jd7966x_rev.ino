/*****************************************************************************
* | File        :   epd_diag_jd7966x_rev.ino
* | Function    :   JD79661 / JD79667 / JD79676 chip revision probe (R70H)
* | Info        :
*----------------
* Port of xiao-esp32c3-blinky/examples/epd_diag_jd7966x_rev.rs (epdsi's
* Spi3Bus::read_jd7966x_revision) to the Seeed Studio XIAO MG24.
*
* Reads the R70H REVISION register of the JD7966x quad-color controllers: command
* 0x70, then three data bytes clocked back on the bidirectional SDA line (the same
* physical wire as MOSI, D10). It never powers the panel on and never refreshes it,
* so the image on the panel is left exactly as it was.
*
* Why it exists: a picture does not identify the chip. The JD79661 init also draws
* a JD79676 panel, and 2.9" panels with different controllers look alike. Two
* units that look identical can read different bytes here, and the sticker alone
* does not settle it.
*
* Seen so far (record new patterns together with the rear sticker text and lot):
*   09 02 01  Seeed 5779, sticker 0213SW-F52-B2 lot NRA2L21 (JD79676AA datasheet)
*   89 02 01  Good Display GDEY0213F52, sticker 0213SW-F52-B2 lot N5B3S02
*   09 01 01  Adafruit, sticker ZJY122250-0213EAH-E5
*   83 02 01  2.9" GDEY029F52, sticker 029SW-F52-B2 lot N2B3F02 (JD79661AA with the
*             top bit set; this unit was ordered as GDEY029F51H)
* Datasheets (section 8.2.22): JD79661AA 03 02 01, JD79667AA 05 02 01,
* JD79676AA 09 02 01.
*
* If the bytes are FF FF FF or 00 00 00 the read path is not reaching the IC. That
* is a wiring or timing result, not a chip result: SDA is the same line as MOSI
* and the driver board may buffer it.
*
* Differences from the Rust example:
*  - Also knows the JD79667AA datasheet value (05 02 01), so an F51H can be told
*    from an F52.
*  - Also compares the low 7 bits, because the units seen so far return the
*    datasheet value with the top bit set.
*  - After the probe the summary is printed again every 10 s (without touching the
*    panel), so a serial monitor attached late still catches it.
*
* Board       :   SiliconLabs:silabs:xiao_mg24:protocol_stack=none
* Connection  :   Seeed ePaper Driver Board for XIAO v2, 24-pin FPC connector
*                 https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
*
* Pinout:
*   RST  -> D0
*   CS   -> D1
*   BUSY -> D2  (Active LOW on JD7966x: 0 = busy, 1 = ready/idle)
*   DC   -> D3
*   SCK  -> D8  (bit-banged here, not the SPI peripheral)
*   SDA  -> D10 (same wire as MOSI; driven as output to write, input to read)
*
* Note: SPI.begin() is deliberately never called, so D8 and D10 stay plain GPIO.
******************************************************************************/
#include <Arduino.h>

#define EPD_RST_PIN   D0
#define EPD_CS_PIN    D1
#define EPD_BUSY_PIN  D2
#define EPD_DC_PIN    D3
#define EPD_SCK_PIN   D8
#define EPD_SDA_PIN   D10

// Datasheet R70H values and units seen on hardware.
static const uint8_t REV_JD79661_DATASHEET[3] = {0x03, 0x02, 0x01};
static const uint8_t REV_JD79667_DATASHEET[3] = {0x05, 0x02, 0x01};
static const uint8_t REV_JD79676_DATASHEET[3] = {0x09, 0x02, 0x01};
static const uint8_t REV_GD_F52_LOT_N5B3S02[3] = {0x89, 0x02, 0x01};
static const uint8_t REV_ADAFRUIT_ZJY122250[3] = {0x09, 0x01, 0x01};
static const uint8_t REV_GD_029F52_LOT_N2B3F02[3] = {0x83, 0x02, 0x01};

static uint8_t first_rev[3] = {0, 0, 0};
static bool probe_done = false;
static bool busy_timeout = false;

static bool same3(const uint8_t *a, const uint8_t *b)
{
  return a[0] == b[0] && a[1] == b[1] && a[2] == b[2];
}

static void print_rev(const uint8_t *r)
{
  for (int i = 0; i < 3; i++) {
    if (r[i] < 0x10) Serial.print('0');
    Serial.print(r[i], HEX);
    if (i < 2) Serial.print(' ');
  }
}

// Polls BUSY (active LOW: busy while LOW). Returns false after 1,500 ms, as epdsi does.
static bool wait_busy()
{
  uint16_t retries = 0;
  while (digitalRead(EPD_BUSY_PIN) == LOW) {
    delay(1);
    if (++retries > 1500) return false;
  }
  return true;
}

// MSB first, SCK toggled once per bit, 1 us pacing (epdsi Spi3Bus::write_byte).
static void write_byte(uint8_t value)
{
  // Load the first bit's level before turning the pin around, so it never drives a stale
  // latch value for an instant after the panel has released the line.
  digitalWrite(EPD_SDA_PIN, (value & 0x80) ? HIGH : LOW);
  pinMode(EPD_SDA_PIN, OUTPUT);
  for (int i = 0; i < 8; i++) {
    digitalWrite(EPD_SDA_PIN, (value & (1 << (7 - i))) ? HIGH : LOW);
    delayMicroseconds(1);
    digitalWrite(EPD_SCK_PIN, HIGH);
    delayMicroseconds(1);
    digitalWrite(EPD_SCK_PIN, LOW);
    delayMicroseconds(1);
  }
}

// Releases SDA, then samples it after each rising SCK edge (epdsi Spi3Bus::read_byte).
static uint8_t read_byte()
{
  pinMode(EPD_SDA_PIN, INPUT);
  uint8_t value = 0;
  for (int i = 0; i < 8; i++) {
    digitalWrite(EPD_SCK_PIN, HIGH);
    delayMicroseconds(1);
    if (digitalRead(EPD_SDA_PIN) == HIGH) value |= (1 << (7 - i));
    digitalWrite(EPD_SCK_PIN, LOW);
    delayMicroseconds(1);
  }
  return value;
}

static void write_cmd(uint8_t cmd)
{
  digitalWrite(EPD_DC_PIN, LOW);
  digitalWrite(EPD_CS_PIN, LOW);
  write_byte(cmd);
  digitalWrite(EPD_CS_PIN, HIGH);
}

static uint8_t read_data_byte()
{
  digitalWrite(EPD_DC_PIN, HIGH);
  digitalWrite(EPD_CS_PIN, LOW);
  uint8_t v = read_byte();
  digitalWrite(EPD_CS_PIN, HIGH);
  return v;
}

// Command 0x70, then three data bytes, each framed by its own CS pulse.
static void read_revision(uint8_t out[3])
{
  write_cmd(0x70);
  for (int i = 0; i < 3; i++) out[i] = read_data_byte();
}

static void print_header()
{
  Serial.println(F("=== diag: JD7966x R70H revision probe ==="));
  Serial.print(F("JD79661AA datasheet [")); print_rev(REV_JD79661_DATASHEET); Serial.println(F("]"));
  Serial.print(F("JD79667AA datasheet [")); print_rev(REV_JD79667_DATASHEET); Serial.println(F("]"));
  Serial.print(F("JD79676AA datasheet [")); print_rev(REV_JD79676_DATASHEET); Serial.println(F("]"));
  Serial.print(F("seen: Good Display 0213 F52 [")); print_rev(REV_GD_F52_LOT_N5B3S02); Serial.println(F("]"));
  Serial.print(F("seen: Adafruit ZJY122250 [")); print_rev(REV_ADAFRUIT_ZJY122250); Serial.println(F("]"));
  Serial.print(F("seen: Good Display 2.9\" GDEY029F52 [")); print_rev(REV_GD_029F52_LOT_N2B3F02); Serial.println(F("]"));
}

static void print_result()
{
  Serial.print(F("Result: "));
  if (busy_timeout) {
    Serial.println(F("BUSY stayed low for 1.5 s, panel hung or wiring wrong. No probe was run."));
    return;
  }
  const uint8_t *f = first_rev;
  uint8_t low7[3] = {(uint8_t)(f[0] & 0x7F), f[1], f[2]};

  if (same3(f, REV_GD_029F52_LOT_N2B3F02)) {
    Serial.println(F("matches the Good Display 2.9\" GDEY029F52 unit seen so far (JD79661AA with the top bit set)"));
  } else if (same3(f, REV_JD79676_DATASHEET)) {
    Serial.println(F("matches the JD79676AA datasheet (seen on Seeed 5779)"));
  } else if (same3(f, REV_GD_F52_LOT_N5B3S02)) {
    Serial.println(F("matches the Good Display 0213 F52 unit seen so far"));
  } else if (same3(f, REV_ADAFRUIT_ZJY122250)) {
    Serial.println(F("matches the Adafruit ZJY122250 unit seen so far"));
  } else if (same3(f, REV_JD79661_DATASHEET)) {
    Serial.println(F("matches the JD79661AA datasheet exactly"));
  } else if (same3(f, REV_JD79667_DATASHEET)) {
    Serial.println(F("matches the JD79667AA datasheet exactly (a GDEY029F51H would read this)"));
  } else if ((f[0] == 0xFF && f[1] == 0xFF && f[2] == 0xFF) ||
             (f[0] == 0x00 && f[1] == 0x00 && f[2] == 0x00)) {
    Serial.print(F("read path not reaching the IC (all "));
    if (f[0] < 0x10) Serial.print('0');
    Serial.print(f[0], HEX);
    Serial.println(F("), see the header comment"));
  } else if (same3(low7, REV_JD79667_DATASHEET)) {
    Serial.println(F("low 7 bits match JD79667AA (05 02 01) with the top bit set: looks like a GDEY029F51H"));
  } else if (same3(low7, REV_JD79661_DATASHEET)) {
    Serial.println(F("low 7 bits match JD79661AA (03 02 01) with the top bit set: looks like a GDEY029F52"));
  } else if (same3(low7, REV_JD79676_DATASHEET)) {
    Serial.println(F("low 7 bits match JD79676AA (09 02 01) with the top bit set"));
  } else {
    Serial.print(F("new pattern ["));
    print_rev(f);
    Serial.println(F("], record it with the sticker text and lot"));
  }
}

void setup()
{
  Serial.begin(115200);
  // Serial is EUSART0 bridged through the on-board CMSIS-DAP chip, so the wait is bounded.
  while (!Serial && millis() < 3000) {
    delay(10);
  }

  // Startup hold, as in the Rust example: attach the serial monitor now, before the
  // panel is touched.
  delay(8000);

  print_header();

  pinMode(EPD_CS_PIN, OUTPUT);   digitalWrite(EPD_CS_PIN, HIGH);
  pinMode(EPD_DC_PIN, OUTPUT);   digitalWrite(EPD_DC_PIN, LOW);
  pinMode(EPD_RST_PIN, OUTPUT);  digitalWrite(EPD_RST_PIN, HIGH);
  pinMode(EPD_SCK_PIN, OUTPUT);  digitalWrite(EPD_SCK_PIN, LOW);
  // Active-LOW BUSY, pulled up so an unpowered panel reads idle.
  pinMode(EPD_BUSY_PIN, INPUT_PULLUP);
  pinMode(EPD_SDA_PIN, INPUT);

  // Reset with the Good Display demo's minimums: 20 ms settle, 40 ms low, 50 ms high.
  delay(20);
  digitalWrite(EPD_RST_PIN, LOW);
  delay(40);
  digitalWrite(EPD_RST_PIN, HIGH);
  delay(50);

  if (!wait_busy()) {
    busy_timeout = true;
    print_result();
    Serial.println(F("Done. The panel was not powered on or refreshed."));
    return;
  }

  for (int attempt = 0; attempt < 3; attempt++) {
    uint8_t rev[3];
    read_revision(rev);
    Serial.print(F("attempt ")); Serial.print(attempt + 1);
    Serial.print(F(": R70H = [")); print_rev(rev); Serial.println(F("]"));
    if (attempt == 0) {
      for (int i = 0; i < 3; i++) first_rev[i] = rev[i];
    }
    delay(10);
  }
  probe_done = true;

  print_result();
  Serial.println(F("Done. The panel was not powered on or refreshed."));
}

void loop()
{
  // Re-print the summary so a late-attached monitor still sees it. The panel is not touched.
  delay(10000);
  if (probe_done || busy_timeout) {
    Serial.print(F("R70H (first read) = ["));
    print_rev(first_rev);
    Serial.println(F("]"));
    print_result();
  }
}
