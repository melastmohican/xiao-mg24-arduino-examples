/*****************************************************************************
* | File        :   GDEY042T81.ino
* | Function    :   4.2inch e-Paper demo - 400x300, black/white monochrome
* | Info        :
*----------------
* Official Good Display GDEY042T81 sample (A32-GDEY042T81) adapted for the
* Seeed Studio XIAO MG24 with the panel connected to the Seeed Studio
* ePaper Driver Board for XIAO v2's 24-pin FPC connector.
*
* Board       :   SiliconLabs:silabs:xiao_mg24:protocol_stack=none
* Panel       :   Good Display GDEY042T81, 400x300, SSD1683
* Connection  :   Seeed ePaper Driver Board for XIAO v2, 24-pin FPC connector
*                 https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
*
* Pinout:
*   RST  -> D0
*   CS   -> D1
*   BUSY -> D2  (Active HIGH on SSD1683: 1 = busy, 0 = idle)
*   DC   -> D3
*   SCK  -> D8
*   MOSI -> D10
******************************************************************************/
#include <SPI.h>
#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "Ap_29demo.h"

// Set to 1 if your physical mounting requires Good Display 180-degree rotation.
// Default 0 matches Good Display standard orientation.
#define EPD_USE_180_ROTATION 0

#if EPD_USE_180_ROTATION
#define EPD_Init_Mode() EPD_HW_Init_180()
#else
#define EPD_Init_Mode() EPD_HW_Init()
#endif

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }
  Serial.println(F("=================================================="));
  Serial.println(F(" Good Display GDEY042T81 Official Sample"));
  Serial.println(F(" XIAO MG24 + Seeed ePaper Driver Board v2"));
  Serial.println(F("=================================================="));

  pinMode(EPD_BUSY_PIN, INPUT);
  pinMode(EPD_RST_PIN,  OUTPUT);
  pinMode(EPD_DC_PIN,   OUTPUT);
  pinMode(EPD_CS_PIN,   OUTPUT);

  EPD_SPI_PORT.begin();
  EPD_SPI_PORT.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
}

void loop() {
  unsigned char i;

  Serial.println(F("1. Full screen clear (white)..."));
  EPD_Init_Mode();
  EPD_WhiteScreen_White();
  EPD_DeepSleep();
  delay(2000);

  Serial.println(F("2. Full screen update: Official Image 1 (gImage_1, ~2.0s)..."));
  EPD_Init_Mode();
  EPD_WhiteScreen_ALL(gImage_1);
  EPD_DeepSleep();
  delay(3000);

  Serial.println(F("3. Fast update mode: Official Image 2 (gImage_2, ~1.5s)..."));
  EPD_HW_Init_Fast();
  EPD_WhiteScreen_ALL_Fast(gImage_2);
  EPD_DeepSleep();
  delay(3000);

  Serial.println(F("4. 4-Gray update mode (gImage_4G1)..."));
  EPD_HW_Init_4G();
  EPD_WhiteScreen_ALL_4G(gImage_4G1);
  EPD_DeepSleep();
  delay(3000);

  Serial.println(F("5. Partial update digital clock sequence..."));
  EPD_Init_Mode();
  EPD_SetRAMValue_BaseMap(gImage_basemap);
  for (i = 0; i < 6; i++) {
    EPD_Dis_Part_Time(152, 92 + 32 * 0, Num[i],
                      152, 92 + 32 * 1, Num[0],
                      152, 92 + 32 * 2, gImage_numdot,
                      152, 92 + 32 * 3, Num[0],
                      152, 92 + 32 * 4, Num[1],
                      32, 64);
    delay(500);
  }
  EPD_DeepSleep();
  delay(2000);

  Serial.println(F("6. Final clear and deep sleep..."));
  EPD_Init_Mode();
  EPD_WhiteScreen_White();
  EPD_DeepSleep();

  Serial.println(F("Demo completed."));
  while (1) {
    delay(1000);
  }
}
