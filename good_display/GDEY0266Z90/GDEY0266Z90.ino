/*****************************************************************************
* | File        :   GDEY0266Z90.ino
* | Function    :   2.66inch e-Paper (B) demo - 152x296, black/white/red
* | Info        :
*----------------
* Good Display's own GDEY0266Z90 sample, adapted to run on the Seeed Studio
* XIAO MG24 with the panel seated in the Seeed Studio ePaper Driver Board for
* XIAO v2's 24-pin FPC connector.
*
* The Waveshare 2.66inch e-Paper (B) is this panel - GxEPD2 aliases the two in
* GxEPD2.h ("GDEY0266Z90, Waveshare_2_66_bwr = GDEY0266Z90"), and the SSD1680
* register sequence here matches the one in this repo's Waveshare_2in66br
* sketch. Of the four drivers in this repo this is the only one that exposes
* the panel's fast-update and partial-update modes.
*
* Board       :   SiliconLabs:silabs:xiao_mg24:protocol_stack=none
* Panel       :   GDEY0266Z90 / Waveshare 2.66inch e-Paper (B), 152x296, SSD1680
* Connection  :   Seeed ePaper Driver Board for XIAO v2, 24-pin FPC connector
*                 https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
*
* Ported from the Adafruit Feather RP2040 ThinkInk version in
* ../../../AdafruitFeatherThinkInk/. Only Display_EPD_W21_spi.h and this file's
* setup() are board-specific.
*
* This is a single-target port: the original's Arduino_UNO / ESP8266 board
* selection has been removed throughout rather than extended with a third
* branch, so the pins, the SPI port and the halt at the end are all
* unconditional. The #if switches that remain are demo options, not board
* options - EPD_INIT_180 below, and the per-section toggles in loop().
*
* Changes from the Good Display original:
*  - Display_EPD_W21_spi.h/.cpp: pins are the driver board's XIAO pins (RST D0,
*    CS D1, BUSY D2, DC D3, SCK D8, MOSI D10) and SPI_Write goes through
*    EPD_SPI_PORT, which is the default SPI here - D8/D10 already are the MG24's
*    hardware SPI. The ESP8266-only Sys_run()/LED_run() helpers are gone.
*  - Display_EPD_W21.cpp: the two #ifdef'd copies of Epaper_READBUSY() collapsed
*    into one with a 40s timeout, so a stalled panel fails loudly instead of
*    spinning forever.
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
*
* Everything else is Good Display's, unchanged.
*
* Timing measured on this panel: full update ~20s, "fast" update also ~19s, and
* the partial-update section ~129s for its 7 refreshes. Tricolor panels need the
* long waveform for red, so neither fast nor partial mode is actually fast here
* - GxEPD2 documents the same panel as "hasPartialUpdate ... but refresh is full
* screen". Set the section toggles in loop() to 0 for a ~40s demo.
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
   Serial.println(F("GDEY0266Z90 (Good Display sample) on XIAO MG24 + ePaper Board v2"));

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


//Tips//
/*
1.Flickering is normal when EPD is performing a full screen update to clear ghosting from the previous image so to ensure better clarity and legibility for the new image.
2.There will be no flicker when EPD performs a partial update.
3.Please make sue that EPD enters sleep mode when update is completed and always leave the sleep mode command. Otherwise, this may result in a reduced lifespan of EPD.
4.Please refrain from inserting EPD to the FPC socket or unplugging it when the MCU is being powered to prevent potential damage.)
5.Re-initialization is required for every full screen update.
6.When porting the program, set the BUSY pin to input mode and other pins to output mode.
*/
void loop() {
   unsigned char i;
#if 1 //Full screen update, fast update, and partial update demostration.

			Serial.println(F("Full screen update..."));
			EPD_HW_Init_Full(); //Full screen update initialization.
		  EPD_WhiteScreen_ALL(gImage_BW1,gImage_RW1); //To Display one image using full screen update.
		  EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
			delay(3000); //Delay for 3s.

    #if 1 // Fast update demostration.
			// No EPD_HW_Init_Fast_180() exists: the fast init issues SWRESET and
			// then only the temperature registers, leaving the RAM window and data
			// entry mode at their power-on defaults. So this pass ignores
			// EPD_INIT_180 and lands in whatever orientation those defaults give.
			Serial.println(F("Fast full screen update..."));
			EPD_HW_Init_Fast(); //Full screen update initialization.
		  EPD_WhiteScreen_ALL_Fast(gImage_BW1,gImage_RW1); //To Display one image using full screen update.
		  EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
			delay(3000); //Delay for 3s.

    #endif
	#if 1 //Partial update demostration.
	// Left on the unrotated EPD_HW_Init() regardless of EPD_INIT_180: the base
	// map and the x/y windows below are authored against the unrotated frame, and
	// rotating the init without also transforming those coordinates would put the
	// partial windows somewhere other than where the base map expects them.
	//Partial update demo support displaying a clock at 5 locations with 00:00.  If you need to perform partial update more than 5 locations, please use the feature of using partial update at the full screen demo.
	//After 5 partial update, implement a full screen update to clear the ghosting caused by partial updatees.
	//////////////////////Partial update time demo/////////////////////////////////////
		  Serial.println(F("Partial update..."));
		  EPD_HW_Init(); //Electronic paper initialization.
			EPD_SetRAMValue_BaseMap(gImage_BWbasemap,gImage_RWbasemap); //Please do not delete the background color function, otherwise it will cause unstable display during partial update.

			for(i=0;i<6;i++)
			EPD_Dis_Part_Num(32,180+32*0,Num[5-i],         //x-A,y-A,DATA-A
												32,180+32*1,Num[i],         //x-B,y-B,DATA-B
												32,180+32*2,gImage_dot, //x-C,y-C,DATA-C
												32,180+32*3,Num[1],32,64); //x-D,y-D,DATA-D,Resolution  32*64
			EPD_Dis_Part_Num(32,180+32*0,Num[8],         //x-A,y-A,DATA-A
										32,180+32*1,Num[1],         //x-B,y-B,DATA-B
										32,180+32*2,gImage_dot, //x-C,y-C,DATA-C
										32,180+32*3,Num[2],32,64); //x-D,y-D,DATA-D,Resolution  32*64										
										
		  EPD_DeepSleep();  //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
      delay(3000); //Delay for 3s.
		//Full screen update clear the screen.
		Serial.println(F("Clear screen..."));
		EPD_HW_Init_Full(); //Full screen update initialization.
		EPD_WhiteScreen_White(); //Clear screen function.
		EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
		delay(3000); //Delay for 3s.
 #endif	
	
	
	// Superseded by EPD_INIT_180 at the top of this file, which routes every
	// full-screen pass through EPD_HW_Init_180() rather than adding a separate
	// one at the end. Kept for reference against Good Display's original.
	#if 0 //Demonstration of full screen update with 180-degree rotation, to enable this feature, please change 0 to 1.
		/************Full display(2s)*******************/
		EPD_HW_Init_180(); //Full screen update initialization.
		EPD_WhiteScreen_ALL(gImage_BW1,gImage_RW1); //To Display one image using full screen update.
		EPD_DeepSleep(); //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
		delay(3000); //Delay for 3s.
	#endif							
#endif

 // Halt. Without this loop() would return and the Arduino core would call it
 // again, restarting the whole demo forever - which is what the original does
 // on any board that is not the UNO or the ESP8266 it guarded this with.
 Serial.println(F("Demo complete - the program stops here."));
 while(1);
}




//////////////////////////////////END//////////////////////////////////////////////////
