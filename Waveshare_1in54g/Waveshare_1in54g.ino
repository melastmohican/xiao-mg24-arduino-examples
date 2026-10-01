/*****************************************************************************
* | File        :   Waveshare_1in54g.ino
* | Function    :   1.54inch e-Paper (G) demo - 200x200, black/white/yellow/red
* | Info        :
*----------------
* Adapted from Waveshare's official 1in54_e-Paper_G demo to run on the
* Seeed Studio XIAO MG24 with the panel seated in the Seeed Studio ePaper Driver
* Board for XIAO v2's 24-pin FPC connector.
*
* Board       :   SiliconLabs:silabs:xiao_mg24:protocol_stack=none
* Panel       :   Waveshare 1.54inch e-Paper (G), SKU 30441, FPC-8101, JD79660AA
* Connection  :   Seeed ePaper Driver Board for XIAO v2, 24-pin FPC connector
*                 https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
*
* Pinout:
*   RST  -> D0
*   CS   -> D1
*   BUSY -> D2  (Active LOW on JD79660)
*   DC   -> D3
*   SCK  -> D8
*   MOSI -> D10
*
* Features:
*  - Displays the official 200x200 4-color test bitmap (Image4color)
*  - Demonstrates in-memory 4-color 2bpp rendering via GUI_Paint
*  - Demonstrates manufacturer fast-update initialization (EPD_1IN54G_Init_Fast)
*  - Deep sleep preservation
******************************************************************************/
#include "EPD_1in54g.h"
#include "GUI_Paint.h"
#include "fonts.h"
#include "ImageData.h"

void setup()
{
    // Initialize hardware and bring up serial
    DEV_Module_Init();
    Serial.println();
    Serial.println(F("=================================================="));
    Serial.println(F(" Waveshare 1.54inch e-Paper (G) Demo (XIAO MG24)  "));
    Serial.println(F("=================================================="));

    Serial.println(F("e-Paper Init and Clear (White)..."));
    EPD_1IN54G_Init();
    EPD_1IN54G_Clear(EPD_1IN54G_WHITE);
    DEV_Delay_ms(1000);

    // 1. Display the built-in 4-color bitmap
    Serial.println(F("Displaying built-in 200x200 4-color bitmap (Image4color)..."));
    EPD_1IN54G_Display(Image4color);
    DEV_Delay_ms(3000);

    // 2. In-memory drawing on the image using GUI_Paint
    UBYTE *BlackImage;
    UWORD Imagesize = ((EPD_1IN54G_WIDTH % 4 == 0) ? (EPD_1IN54G_WIDTH / 4) : (EPD_1IN54G_WIDTH / 4 + 1)) * EPD_1IN54G_HEIGHT;
    if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        Serial.println(F("Failed to allocate image memory!"));
        while (1);
    }

    Serial.println(F("Drawing 4-color graphics with GUI_Paint..."));
    Paint_NewImage(BlackImage, EPD_1IN54G_WIDTH, EPD_1IN54G_HEIGHT, 0, EPD_1IN54G_WHITE);
    Paint_SetScale(4);
    Paint_SelectImage(BlackImage);
    Paint_Clear(EPD_1IN54G_WHITE);

    // Decorative header bar
    Paint_DrawRectangle(0, 0, 200, 24, EPD_1IN54G_RED, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawString_EN(12, 4, "1.54\" 4-Color (G)", &Font16, EPD_1IN54G_WHITE, EPD_1IN54G_RED);

    // Color text examples
    Paint_DrawString_EN(10, 32, "Red, Yellow,", &Font16, EPD_1IN54G_RED, EPD_1IN54G_WHITE);
    Paint_DrawString_EN(10, 50, "White and Black", &Font16, EPD_1IN54G_YELLOW, EPD_1IN54G_WHITE);
    Paint_DrawString_EN(10, 68, "JD79660 Controller", &Font12, EPD_1IN54G_BLACK, EPD_1IN54G_WHITE);

    // Geometric primitives
    Paint_DrawPoint(15, 92, EPD_1IN54G_RED, DOT_PIXEL_2X2, DOT_STYLE_DFT);
    Paint_DrawPoint(25, 92, EPD_1IN54G_YELLOW, DOT_PIXEL_2X2, DOT_STYLE_DFT);
    Paint_DrawPoint(35, 92, EPD_1IN54G_BLACK, DOT_PIXEL_2X2, DOT_STYLE_DFT);

    // Lines & Rectangles
    Paint_DrawLine(10, 105, 90, 105, EPD_1IN54G_RED, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(10, 110, 90, 110, EPD_1IN54G_YELLOW, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(10, 115, 90, 115, EPD_1IN54G_BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);

    Paint_DrawRectangle(10, 125, 50, 155, EPD_1IN54G_YELLOW, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawRectangle(55, 125, 95, 155, EPD_1IN54G_RED, DOT_PIXEL_1X1, DRAW_FILL_FULL);

    // Circles
    Paint_DrawCircle(145, 125, 24, EPD_1IN54G_BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawCircle(145, 125, 18, EPD_1IN54G_RED, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawCircle(145, 125, 10, EPD_1IN54G_YELLOW, DOT_PIXEL_1X1, DRAW_FILL_FULL);

    // Chinese typography & numeric output
    Paint_DrawString_CN(10, 165, "微雪电子", &Font24CN, EPD_1IN54G_RED, EPD_1IN54G_WHITE);
    Paint_DrawNum(120, 172, 2026, &Font16, EPD_1IN54G_BLACK, EPD_1IN54G_WHITE);

    // Send rendered buffer to display using fast-init update
    Serial.println(F("Displaying rendered graphics (Fast Init)..."));
    EPD_1IN54G_Init_Fast();
    EPD_1IN54G_Display(BlackImage);
    DEV_Delay_ms(3000);

    Serial.println(F("Entering deep sleep..."));
    EPD_1IN54G_Sleep();
    free(BlackImage);
    BlackImage = NULL;
    DEV_Delay_ms(2000);

    DEV_Module_Exit();
    Serial.println(F("Demo complete."));
}

void loop()
{
    // Standalone setup demo
}
