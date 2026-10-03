/*****************************************************************************
* | File        :   GDEW0215T12.ino
* | Function    :   2.15inch e-Paper demo - 208x112, black/white monochrome
* | Info        :
*----------------
* Good Display's official GDEW0215T12 sample (AU-GDEW0215T12), adapted to run
* on the Seeed Studio XIAO MG24 with the panel seated in the Seeed Studio
* ePaper Driver Board for XIAO v2's 24-pin FPC connector.
*
* Board       :   SiliconLabs:silabs:xiao_mg24:protocol_stack=none
* Panel       :   GDEW0215T12 (formerly GDEW0215T11), 208x112, UC8151D controller
* Connection  :   Seeed ePaper Driver Board for XIAO v2, 24-pin FPC connector
*                 https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
*
* Pinout:
*   RST  -> D0
*   CS   -> D1
*   BUSY -> D2  (Active LOW on UC8151D: 0 = busy, 1 = ready)
*   DC   -> D3
*   SCK  -> D8
*   MOSI -> D10
******************************************************************************/
#include <SPI.h>
#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "Ap_29demo.h"  

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  Serial.println(F("GDEW0215T12 (Good Display sample) on XIAO MG24 + ePaper Board v2"));

  pinMode(EPD_BUSY_PIN, INPUT);
  pinMode(EPD_RST_PIN,  OUTPUT);
  pinMode(EPD_DC_PIN,   OUTPUT);
  pinMode(EPD_CS_PIN,   OUTPUT);

  // Default SPI on XIAO MG24 uses D8 (SCK) and D10 (MOSI)
  EPD_SPI_PORT.begin();
  EPD_SPI_PORT.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
}

void loop() {
  unsigned char i;

  Serial.println(F("1. Full screen clear (white)..."));
  EPD_Init();
  EPD_WhiteScreen_White();
  EPD_DeepSleep();
  delay(2000);

  Serial.println(F("2. Full screen image display (gImage_1)..."));
  EPD_Init();
  EPD_WhiteScreen_ALL(gImage_1);
  EPD_DeepSleep();
  delay(2000);

  Serial.println(F("3. Partial refresh clock demonstration..."));
  EPD_Init();
  EPD_SetRAMValue_BaseMap(gImage_basemap);
  for (i = 0; i < 6; i++) {
    EPD_Dis_Part_Time(48, 46, Num[i], Num[0], gImage_numdot, Num[0], Num[1], 5, 24, 32);
    delay(500);
  }
  EPD_DeepSleep();
  delay(2000);

  Serial.println(F("4. Full screen clear and enter deep sleep..."));
  EPD_Init();
  EPD_WhiteScreen_White();
  EPD_DeepSleep();

  Serial.println(F("Demo completed. Halted."));
  while (1) {
    delay(1000);
  }
}
