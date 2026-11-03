/*****************************************************************************
* | File        :   GDEW0102I4FC.ino
* | Function    :   1.02inch Monochrome e-Paper demo, 80x128, Black/White
* | Info        :
*----------------
* Good Display official GDEW0102I4FC sample (UC8175 controller), adapted to
* run on the Seeed Studio XIAO MG24 with the DESPI-C102 adapter board.
*
* Board       :   SiliconLabs:silabs:xiao_mg24:protocol_stack=none
* Panel       :   Good Display GDEW0102I4FC, 80x128, UC8175 controller, flexible
* Adapter     :   Good Display DESPI-C102_30P connector board
*
* Pinout (Direct 3.3V connection, no level shifter needed):
*   RST  -> D0
*   CS   -> D1
*   BUSY -> D2  (Active LOW: 0 = busy, 1 = ready/idle)
*   DC   -> D3
*   SCK  -> D8
*   MOSI -> D10
*   GND  -> GND
*   VDD  -> 3V3
******************************************************************************/
#include <SPI.h>
#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "Ap_29demo.h"

void setup()
{
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  Serial.println();
  Serial.println(F("=================================================="));
  Serial.println(F(" GDEW0102I4FC Good Display Demo on XIAO MG24      "));
  Serial.println(F("=================================================="));

  pinMode(EPD_BUSY_PIN, INPUT);
  pinMode(EPD_RST_PIN,  OUTPUT);
  pinMode(EPD_DC_PIN,   OUTPUT);
  pinMode(EPD_CS_PIN,   OUTPUT);

  // EPD SPI transaction opened once for sketch lifetime on Silabs core
  EPD_SPI_PORT.begin();
  EPD_SPI_PORT.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
}

void loop()
{
  // 1. Clear screen to white
  Serial.println(F("1. Clearing screen to White..."));
  EPD_Init();
  EPD_WhiteScreen_White();
  EPD_DeepSleep();
  Serial.println(F("   Done. Waiting 2s..."));
  delay(2000);

  // 2. Full screen update with demo bitmap 1
  Serial.println(F("2. Full screen update (gImage_1)..."));
  EPD_Init();
  EPD_WhiteScreen_ALL(gImage_1);
  EPD_DeepSleep();
  Serial.println(F("   Done. Waiting 2s..."));
  delay(2000);

  // 3. Full screen update with demo bitmap 2
  Serial.println(F("3. Full screen update (gImage_2)..."));
  EPD_Init();
  EPD_WhiteScreen_ALL(gImage_2);
  EPD_DeepSleep();
  Serial.println(F("   Done. Waiting 2s..."));
  delay(2000);

  // 4. Return to clean white
  Serial.println(F("4. Returning screen to White..."));
  EPD_Init();
  EPD_WhiteScreen_White();
  EPD_DeepSleep();
  Serial.println(F("   Done. Demo complete."));

  // Halt execution
  while (1) {
    delay(1000);
  }
}

