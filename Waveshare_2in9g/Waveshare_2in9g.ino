/*****************************************************************************
* | File        :   Waveshare_2in9g.ino
* | Function    :   2.9inch e-Paper (G) demo - 296x128, black/white/yellow/red
* | Info        :
*----------------
* Adapted from Waveshare's official 2.9inch e-Paper (G) Arduino_R4 demo
* (2.9inch_e-Paper_G.zip, EPD_2in9g V1.0 2025-12-02) to run on the Seeed Studio
* XIAO MG24 with the panel seated in the Seeed Studio ePaper Driver Board for
* XIAO v2's 24-pin FPC connector.
*
* Board       :   SiliconLabs:silabs:xiao_mg24:protocol_stack=none
* Panel       :   Waveshare 2.9inch e-Paper (G), 296x128 (128x296 native),
*                 same glass and JD79661 init as Good Display GDEY029F52
* Connection  :   Seeed ePaper Driver Board for XIAO v2, 24-pin FPC connector
*                 https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
*
* Pinout:
*   RST  -> D0
*   CS   -> D1
*   BUSY -> D2  (Active LOW on JD79661: 0 = busy, 1 = ready/idle)
*   DC   -> D3
*   SCK  -> D8
*   MOSI -> D10
*
* Features:
*  - Displays the official 296x128 4-color test bitmap (Image4color) with the
*    manufacturer fast-update init (EPD_2IN9G_Init_Fast)
*  - Demonstrates in-memory 4-color 2bpp rendering via GUI_Paint (rotated 90
*    degrees, so the canvas is 296 wide by 128 tall)
*  - Deep sleep after the demo
******************************************************************************/
#include "EPD_2in9g.h"
#include "GUI_Paint.h"
#include "fonts.h"
#include "ImageData.h"

void setup()
{
    // Initialize hardware and bring up serial
    DEV_Module_Init();
    Serial.println();
    Serial.println(F("=================================================="));
    Serial.println(F(" Waveshare 2.9inch e-Paper (G) Demo (XIAO MG24)   "));
    Serial.println(F("=================================================="));

    Serial.println(F("e-Paper Init and Clear (White)..."));
    EPD_2IN9G_Init();
    EPD_2IN9G_Clear(EPD_2IN9G_WHITE);
    DEV_Delay_ms(2000);

    // Create a new image cache
    UBYTE *BlackImage;
    UDOUBLE Imagesize = ((EPD_2IN9G_WIDTH % 4 == 0) ? (EPD_2IN9G_WIDTH / 4) : (EPD_2IN9G_WIDTH / 4 + 1)) * EPD_2IN9G_HEIGHT;
    if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        Serial.println(F("Failed to allocate image memory!"));
        while (1);
    }
    Paint_NewImage(BlackImage, EPD_2IN9G_WIDTH, EPD_2IN9G_HEIGHT, 0, EPD_2IN9G_WHITE);
    Paint_SetScale(4);

    // 1. Display the built-in 4-color bitmap (fast update)
    Serial.println(F("Displaying built-in 296x128 4-color bitmap (Image4color, fast init)..."));
    EPD_2IN9G_Init_Fast();
    EPD_2IN9G_Display(Image4color);
    EPD_2IN9G_Sleep();
    DEV_Delay_ms(3000);

    // 2. In-memory drawing on the image using GUI_Paint
    Serial.println(F("Drawing 4-color graphics with GUI_Paint..."));
    EPD_2IN9G_Init();
    Paint_NewImage(BlackImage, EPD_2IN9G_WIDTH, EPD_2IN9G_HEIGHT, 90, EPD_2IN9G_WHITE);
    Paint_SetScale(4);
    Paint_SelectImage(BlackImage);
    Paint_Clear(EPD_2IN9G_WHITE);

    Paint_DrawPoint(10, 80, EPD_2IN9G_RED, DOT_PIXEL_1X1, DOT_STYLE_DFT);
    Paint_DrawPoint(10, 90, EPD_2IN9G_YELLOW, DOT_PIXEL_2X2, DOT_STYLE_DFT);
    Paint_DrawPoint(10, 100, EPD_2IN9G_BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);
    Paint_DrawLine(20, 70, 70, 120, EPD_2IN9G_RED, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(70, 70, 20, 120, EPD_2IN9G_RED, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawRectangle(20, 70, 70, 120, EPD_2IN9G_YELLOW, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawRectangle(80, 70, 130, 120, EPD_2IN9G_YELLOW, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawCircle(45, 95, 20, EPD_2IN9G_BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawCircle(105, 95, 20, EPD_2IN9G_BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawLine(85, 95, 125, 95, EPD_2IN9G_RED, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(105, 75, 105, 115, EPD_2IN9G_YELLOW, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawString_EN(10, 0, "Red,yellow,white and black", &Font16, EPD_2IN9G_RED, EPD_2IN9G_YELLOW);
    Paint_DrawString_EN(10, 20, "Four color e-Paper", &Font12, EPD_2IN9G_YELLOW, EPD_2IN9G_BLACK);
    Paint_DrawString_CN(150, 20, "微雪电子", &Font24CN, EPD_2IN9G_RED, EPD_2IN9G_WHITE);
    Paint_DrawString_CN(150, 60, "你好abc", &Font12CN, EPD_2IN9G_YELLOW, EPD_2IN9G_BLACK);
    Paint_DrawNum(10, 35, 123456, &Font12, EPD_2IN9G_RED, EPD_2IN9G_WHITE);

    Serial.println(F("EPD_Display..."));
    EPD_2IN9G_Display(BlackImage);
    DEV_Delay_ms(3000);

    // 3. Clear and sleep
    Serial.println(F("Clear and go to sleep..."));
    EPD_2IN9G_Init();
    EPD_2IN9G_Clear(EPD_2IN9G_WHITE);
    EPD_2IN9G_Sleep();
    free(BlackImage);
    BlackImage = NULL;
    DEV_Delay_ms(2000);  // important, at least 2s
    DEV_Module_Exit();

    Serial.println(F("Demo completed."));
}

void loop()
{
    // Demo runs once in setup()
}
