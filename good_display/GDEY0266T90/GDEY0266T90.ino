/*****************************************************************************
* | File        :   GDEY0266T90.ino
* | Function    :   2.66inch e-Paper demo - 152x296, black/white monochrome
* | Info        :
*----------------
* Good Display's own GDEY0266T90 sample, adapted to run on the Seeed Studio
* XIAO MG24 with the panel seated in the Seeed Studio ePaper Driver Board for
* XIAO v2's 24-pin FPC connector.
*
* The Waveshare 2.66inch e-Paper (monochrome) is this panel - GxEPD2 uses
* GxEPD2_266_GDEY0266T90 for both. It is 296x152 on an SSD1680 controller.
* Of the four driver paths in this repo, this Good Display vendor sample is the
* one that exposes the manufacturer's official fast-refresh (1.5s and 1.0s)
* and partial-refresh waveforms.
*
* Board       :   SiliconLabs:silabs:xiao_mg24:protocol_stack=none
* Panel       :   GDEY0266T90 / Waveshare 2.66inch e-Paper (SKU 18401), 152x296, SSD1680
* Connection  :   Seeed ePaper Driver Board for XIAO v2, 24-pin FPC connector
*                 https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
*
* Changes from the Good Display original:
*  - Display_EPD_W21_spi.h/.cpp: pins are the driver board's XIAO pins (RST D0,
*    CS D1, BUSY D2, DC D3, SCK D8, MOSI D10) and SPI_Write goes through
*    EPD_SPI_PORT, which is the default SPI here - D8/D10 already are the MG24's
*    hardware SPI. The ESP8266-only Sys_run()/LED_run() helpers are gone.
*  - Display_EPD_W21.cpp: Epaper_READBUSY() given a 40s timeout, so a stalled
*    panel fails loudly instead of spinning forever.
*  - This file: SPI begin() now precedes beginTransaction() - the original calls
*    them the other way round - and that one transaction is never closed, because
*    SilabsSPI::beginTransaction() takes a FreeRTOS mutex with portMAX_DELAY and
*    returns still holding it, so a second call would deadlock. The clock is
*    4 MHz rather than Good Display's 10 MHz, matching every other XIAO sketch in
*    this repo. Plus Serial for progress, and the full-screen passes use Good
*    Display's own EPD_HW_Init_180() (see EPD_INIT_180 below).
*    The halt at the end of loop() is unconditional; the original guards it with
*    #ifdef Arduino_UNO, so on any other board loop() returns and the core runs
*    the entire demo again, forever.
******************************************************************************/
#include <SPI.h>
//EPD
#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "Ap_29demo.h"

// This panel's origin is the corner opposite the one content should start from
// - a panel/FPC fact, not a board one - so the stock init puts the image 180
// degrees round. Good Display ships a rotated init for exactly this, so unlike
// the other 2.66" sketches in this repo no image-flipping code is needed - it
// is just the other init sequence (data entry 0x02 and a mirrored RAM window).
// Set to 0 for Good Display's default orientation.
#define EPD_INIT_180  1

#if EPD_INIT_180
#define EPD_HW_Init_Full()  EPD_HW_Init_180()
#else
#define EPD_HW_Init_Full()  EPD_HW_Init()
#endif

void setup() {
   Serial.begin(115200);
   // Serial is EUSART0 via the on-board CMSIS-DAP chip, not USB CDC, so bound
   // the wait - it must never block.
   while (!Serial && millis() < 3000) {
     delay(10);
   }
   Serial.println(F("GDEY0266T90 (Good Display sample) on XIAO MG24 + ePaper Board v2"));

   pinMode(EPD_BUSY_PIN, INPUT);  //BUSY
   pinMode(EPD_RST_PIN,  OUTPUT); //RES
   pinMode(EPD_DC_PIN,   OUTPUT); //DC
   pinMode(EPD_CS_PIN,   OUTPUT); //CS

   //SPI
   // The EPD signals are already on the default SPI (D8=SCK, D10=MOSI), so there
   // is nothing to remap. begin() first: the original calls beginTransaction()
   // before begin(), which configures a port that does not exist yet. That one
   // transaction is then left open for the lifetime of the sketch, because
   // SilabsSPI::beginTransaction() takes a FreeRTOS mutex with portMAX_DELAY and
   // returns while still holding it - a second call deadlocks.
   EPD_SPI_PORT.begin ();
   EPD_SPI_PORT.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
}

void loop() {
   unsigned char i;
#if 1 //Full screen refresh, fast refresh, and partial refresh demonstration.

      Serial.println(F("Full screen clear (white)..."));
      EPD_HW_Init_Full(); //Full screen refresh initialization.
      EPD_WhiteScreen_White(); //Clear screen function.
      EPD_DeepSleep(); //Enter sleep mode
      delay(2000);

      /************Full display(2s)*******************/
      Serial.println(F("Full screen update (Image 1)..."));
      EPD_HW_Init_Full();
      EPD_WhiteScreen_ALL(gImage_1); //To Display one image using full screen refresh.
      EPD_DeepSleep();
      delay(2000);

      /************Fast refresh mode(1.5s)*******************/
      Serial.println(F("Fast refresh mode 1 (1.5s)..."));
      EPD_HW_Init_Fast();
      EPD_WhiteScreen_ALL_Fast(gImage_1);
      EPD_DeepSleep();
      delay(2000);

      EPD_HW_Init_Fast();
      EPD_WhiteScreen_ALL_Fast(gImage_2);
      EPD_DeepSleep();
      delay(2000);

      /************Fast refresh mode(1s)*******************/
      Serial.println(F("Fast refresh mode 2 (1.0s)..."));
      EPD_HW_Init_Fast2();
      EPD_WhiteScreen_ALL_Fast2(gImage_1);
      EPD_DeepSleep();
      delay(1000);

      EPD_HW_Init_Fast2();
      EPD_WhiteScreen_ALL_Fast2(gImage_2);
      EPD_DeepSleep();
      delay(1000);

      /************Partial refresh demonstration*******************/
      Serial.println(F("Partial refresh clock demo..."));
      EPD_HW_Init(); //Electronic paper initialization.
      EPD_SetRAMValue_BaseMap(gImage_basemap); //Background color basemap
      for(i=0;i<6;i++) {
        EPD_Dis_Part_Time(48,92+32*0,Num[i],         //x-A,y-A,DATA-A
                          48,92+32*1,Num[0],         //x-B,y-B,DATA-B
                          48,92+32*2,gImage_numdot, //x-C,y-C,DATA-C
                          48,92+32*3,Num[0],        //x-D,y-D,DATA-D
                          48,92+32*4,Num[1],32,64); //x-E,y-E,DATA-E,Resolution 32*64
        delay(500);
      }

      EPD_DeepSleep();
      delay(2000);

      Serial.println(F("Clear screen..."));
      EPD_HW_Init_Full();
      EPD_WhiteScreen_White();
      EPD_DeepSleep();
      delay(2000);
#endif

   Serial.println(F("Demo complete - the program stops here."));
   while(1);
}
