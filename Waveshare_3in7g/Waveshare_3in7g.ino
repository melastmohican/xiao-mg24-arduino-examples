/*****************************************************************************
* | File        :   Waveshare_3in7g.ino
* | Function    :   3.7inch e-Paper (G) demo - 240x416, black/white/yellow/red
* | Info        :
*----------------
* Adapted from Waveshare's official 3in7_e-Paper_G demo to run on the
* Seeed Studio XIAO MG24 with the panel seated in the Seeed Studio ePaper Driver
* Board for XIAO v2's 24-pin FPC connector.
*
* Board       :   SiliconLabs:silabs:xiao_mg24:protocol_stack=none
* Panel       :   Waveshare 3.7inch e-Paper (G), SKU 31065, FPC-2303, IST7163
* Connection  :   Seeed ePaper Driver Board for XIAO v2, 24-pin FPC connector
*                 https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
*
* Pinout:
*   RST  -> D0
*   CS   -> D1
*   BUSY -> D2  (Active LOW on IST7163)
*   DC   -> D3
*   SCK  -> D8
*   MOSI -> D10
*
* Features:
*  - Demonstrates in-memory 4-color 2bpp rendering via GUI_Paint (240x416)
*  - Demonstrates official fast-update initialization (EPD_3IN7G_Init_Fast)
*  - Deep sleep preservation
******************************************************************************/
#include "EPD_3in7g.h"
#include "GUI_Paint.h"
#include "fonts.h"
#include "ImageData.h"

void setup()
{
    // Initialize hardware and bring up serial
    DEV_Module_Init();
    Serial.println();
    Serial.println(F("=================================================="));
    Serial.println(F(" Waveshare 3.7inch e-Paper (G) Demo (XIAO MG24)   "));
    Serial.println(F("=================================================="));

    Serial.println(F("e-Paper Init and Clear (White)..."));
    EPD_3IN7G_Init();
    EPD_3IN7G_Clear(EPD_3IN7G_WHITE);
    DEV_Delay_ms(1000);

    // 1. In-memory drawing on the image using GUI_Paint
    UBYTE *BlackImage;
    UWORD Imagesize = ((EPD_3IN7G_WIDTH % 4 == 0) ? (EPD_3IN7G_WIDTH / 4) : (EPD_3IN7G_WIDTH / 4 + 1)) * EPD_3IN7G_HEIGHT;
    if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        Serial.println(F("Failed to allocate image memory!"));
        while (1);
    }

    Serial.println(F("Drawing 4-color demo primitives..."));
    Paint_NewImage(BlackImage, EPD_3IN7G_WIDTH, EPD_3IN7G_HEIGHT, 0, EPD_3IN7G_WHITE);
    Paint_SetScale(4);
    Paint_Clear(EPD_3IN7G_WHITE);

    // Title header
    Paint_DrawRectangle(10, 10, 230, 60, EPD_3IN7G_BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawString_EN(20, 25, "XIAO MG24 3.7\" G", &Font16, EPD_3IN7G_BLACK, EPD_3IN7G_WHITE);

    // Color bands
    Paint_DrawRectangle(10, 80, 230, 110, EPD_3IN7G_BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawString_EN(20, 88, "BLACK 00", &Font16, EPD_3IN7G_BLACK, EPD_3IN7G_WHITE);

    Paint_DrawRectangle(10, 120, 230, 150, EPD_3IN7G_YELLOW, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawString_EN(20, 128, "YELLOW 10", &Font16, EPD_3IN7G_YELLOW, EPD_3IN7G_BLACK);

    Paint_DrawRectangle(10, 160, 230, 190, EPD_3IN7G_RED, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawString_EN(20, 168, "RED 11", &Font16, EPD_3IN7G_RED, EPD_3IN7G_WHITE);

    // Geometric shapes
    Paint_DrawCircle(60, 250, 40, EPD_3IN7G_RED, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    Paint_DrawCircle(60, 250, 25, EPD_3IN7G_YELLOW, DOT_PIXEL_1X1, DRAW_FILL_FULL);

    Paint_DrawRectangle(130, 210, 210, 290, EPD_3IN7G_BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    Paint_DrawLine(130, 210, 210, 290, EPD_3IN7G_RED, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
    Paint_DrawLine(130, 290, 210, 210, EPD_3IN7G_YELLOW, DOT_PIXEL_2X2, LINE_STYLE_SOLID);

    // Text details
    Paint_DrawString_EN(15, 315, "Resolution: 240x416", &Font12, EPD_3IN7G_WHITE, EPD_3IN7G_BLACK);
    Paint_DrawString_EN(15, 335, "Controller: IST7163", &Font12, EPD_3IN7G_WHITE, EPD_3IN7G_BLACK);
    Paint_DrawString_EN(15, 355, "Panel: GDEM037F51", &Font12, EPD_3IN7G_WHITE, EPD_3IN7G_BLACK);
    Paint_DrawString_EN(15, 375, "Driver: Waveshare C", &Font12, EPD_3IN7G_WHITE, EPD_3IN7G_RED);

    Serial.println(F("Sending image buffer to e-Paper (full refresh ~20s)..."));
    EPD_3IN7G_Display(BlackImage);
    DEV_Delay_ms(5000);

    // 2. Fast refresh demo
    Serial.println(F("Testing Fast Refresh Mode (EPD_3IN7G_Init_Fast)..."));
    EPD_3IN7G_Init_Fast();
    Paint_DrawString_EN(15, 395, "Fast Init Checked", &Font12, EPD_3IN7G_WHITE, EPD_3IN7G_YELLOW);
    EPD_3IN7G_Display(BlackImage);
    DEV_Delay_ms(5000);

    // 3. Power off and sleep
    Serial.println(F("Putting display into deep sleep..."));
    EPD_3IN7G_Sleep();
    free(BlackImage);
    BlackImage = NULL;

    Serial.println(F("Demo completed successfully. Program halted."));
}

void loop()
{
    // Execution halted after setup
    DEV_Delay_ms(1000);
}
