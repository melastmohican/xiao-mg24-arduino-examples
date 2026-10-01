/*****************************************************************************
* | File        :   Waveshare_3in52.ino
* | Function    :   3.52inch e-Paper (B) demo - 240x360, black/white/red
* | Info        :
*----------------
* Adapted from Waveshare's original Arduino_R4.ino, taken from
*   e-Paper/E-paper_Separate_Program/3in52_e-Paper_B/Arduino_R4/
* to run on the Seeed Studio XIAO MG24 with the panel seated in the Seeed
* Studio ePaper Driver Board for XIAO v2's 24-pin FPC connector.
*
* Board       :   SiliconLabs:silabs:xiao_mg24:protocol_stack=none
* Panel       :   Waveshare 3.52inch e-Paper (B), FPC SE0352N01FPC-A, 240x360 BWR
* Connection  :   Seeed ePaper Driver Board for XIAO v2, 24-pin FPC connector
*                 https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
*
* GxEPD2 1.6.9 has no 240x360 panel class, so this vendored Waveshare driver is
* what actually drives the panel. See ../Adafruit_EPD/XIAO_Waveshare_3in52/ for
* the same panel through Adafruit_EPD's UC8253 driver.
*
* Changes from the Waveshare original:
*  - DEV_Config.h/.cpp: pins switched to the driver board's mapping (RST D0,
*    CS D1, BUSY D2, DC D3, SCK D8, MOSI D10), on the default SPI - D8/D10 are
*    already the MG24's hardware SCK/MOSI, so there is nothing to remap.
*    EPD_PWR_PIN dropped: the driver board has no software power gate, so
*    DEV_Module_Exit() is a no-op. DEV_SPI_Init() no longer opens a second SPI
*    transaction, which would deadlock on this core.
*  - EPD_3in52b.cpp: EPD_3IN52B_sleep() now sends POWER_OFF (0x02) and waits
*    for BUSY before deep sleep, and EPD_3IN52B_ReadBusy() has a 40s timeout
*    so a stalled panel fails loudly instead of hanging forever.
*  - This file: printf() -> Serial.printf(). The Silicon Labs core provides
*    UARTClass::printf, but with a 128-byte buffer, so keep the strings short.
*    The banner moved after DEV_Module_Init(), which is what calls
*    Serial.begin(). Paint rotation 90 -> 270 so the drawn demo matches the
*    orientation of the built-in gImage bitmap. The closing Clear() is disabled
*    so the demo ends on the drawn image instead of a blank panel.
*
* Everything else is Waveshare's, unchanged; their licence below applies.
******************************************************************************/
#include "EPD_3in52b.h"
#include "GUI_Paint.h"
#include "fonts.h"
#include "ImageData.h"

void setup() {
    DEV_Module_Init();	// brings up Serial, so the banner has to come after it
    Serial.printf("EPD_3IN52B_test Demo\r\n");

    Serial.printf("e-Paper Init and Clear...\r\n");
    EPD_3IN52B_Init();
    EPD_3IN52B_Clear();
    DEV_Delay_ms(500);


    //Create a new image cache named IMAGE_BW and fill it with white
    UBYTE *Image;
    UWORD Imagesize = ((EPD_3IN52B_WIDTH % 8 == 0) ? (EPD_3IN52B_WIDTH / 8 ) : (EPD_3IN52B_WIDTH / 8 + 1)) * EPD_3IN52B_HEIGHT;
    if ((Image = (UBYTE *)malloc(Imagesize)) == NULL) {
        Serial.printf("Failed to apply for black memory...\r\n");
        while(1);
    }
    Serial.printf("NewImage:BlackImage and RYImage\r\n");
    // 270, not Waveshare's 90: at 90 the drawn demo lands 180 degrees opposed to
    // the built-in gImage bitmap, which is authored as landscape artwork stored
    // sideways in the portrait raster.
    Paint_NewImage(Image, EPD_3IN52B_WIDTH, EPD_3IN52B_HEIGHT , 270, WHITE);

    //Select Image
    Paint_SelectImage(Image);
    Paint_Clear(WHITE);

#if 1   // show image for array    
    Serial.printf("show image for array\r\n");
    EPD_3IN52B_Init();
    EPD_3IN52B_Display(gImage_B, gImage_R);
    DEV_Delay_ms(2000);
#endif

#if 1   // Drawing on the image
    /*Horizontal screen*/
    //1.Draw black image
    EPD_3IN52B_Init();
    Paint_SelectImage(Image);
    Paint_Clear(WHITE);
    Paint_DrawPoint(10, 80, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
    Paint_DrawPoint(10, 90, BLACK, DOT_PIXEL_2X2, DOT_STYLE_DFT);
    Paint_DrawPoint(10, 100, BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);
    Paint_DrawPoint(10, 110, BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);
    Paint_DrawLine(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(70, 70, 20, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawRectangle(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawRectangle(80, 70, 130, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawString_EN(10, 0, "waveshare", &Font16, BLACK, WHITE);
    Paint_DrawString_CN(130, 20, "微雪电子", &Font24CN, WHITE, BLACK);
    Paint_DrawNum(10, 50, 987654321, &Font16, WHITE, BLACK);
    EPD_3IN52B_Display_NUM(Image, 0);

    //2.Draw red image
    Paint_SelectImage(Image);
    Paint_Clear(WHITE);
    Paint_DrawCircle(160, 95, 20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawCircle(210, 95, 20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawLine(85, 95, 125, 95, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(105, 75, 105, 115, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawString_CN(130, 0, "你好abc", &Font12CN, BLACK, WHITE);
    Paint_DrawString_EN(10, 20, "hello world", &Font12, WHITE, BLACK);
    Paint_DrawNum(10, 33, 123456789, &Font12, BLACK, WHITE);

    Serial.printf("EPD_Display\r\n");
    EPD_3IN52B_Display_NUM(Image, 1);
    DEV_Delay_ms(2000);
#endif

    // Left off deliberately: ending on the drawn image rather than wiping to
    // white. A cleared BWR panel looks grey next to a mono one - that is the
    // panel's white point, not a fault. Flip to 1 for Waveshare's behaviour.
#if 0
    Debug("Clear...\r\n");
    EPD_3IN52B_Init();
    EPD_3IN52B_Clear();
    DEV_Delay_ms(2000);
#endif

    Debug("Goto Sleep...\r\n");
    EPD_3IN52B_sleep();
    free(Image);
    DEV_Delay_ms(2000);

    // release the module (no power gate on the Seeed driver board)
    Debug("Panel is asleep, done.\r\n");
    DEV_Module_Exit();
}


void loop() {

}