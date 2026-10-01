/**
 *  @filename   :   epd2in66b.cpp
 *  @brief      :   Implements for e-paper library
 *  @author     :   Waveshare
 *
 *  Copyright (C) Waveshare     Dec 02 2020
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdlib.h>
#include "epd2in66b.h"
#include "imagedata.h"

Epd::~Epd() {
};

Epd::Epd() {
    reset_pin = RST_PIN;
    dc_pin = DC_PIN;
    cs_pin = CS_PIN;
    busy_pin = BUSY_PIN;
    width = EPD_WIDTH;
    height = EPD_HEIGHT;
};

/**
 *  @brief: module reset.
 *          often used to awaken the module in deep sleep,
 *          see Epd::Sleep();
 */
// (`static` dropped from this and the four other out-of-class definitions below:
//  they are declared non-static in epd2in66b.h, and C++ does not allow the
//  storage class to be repeated on a member definition. The vendor copy does not
//  compile as shipped.)
void Epd::Reset(void) {
    DigitalWrite(reset_pin, HIGH);
    DelayMs(200);   
    DigitalWrite(reset_pin, LOW);                //module reset    
    DelayMs(2);
    DigitalWrite(reset_pin, HIGH);
    DelayMs(200);    
}

/**
 *  @brief: Initialize the e-Paper register
 */
int Epd::Init(void) {
	if (IfInit() != 0) {
	  return -1;
	}
	Reset();

	WaitUntilIdle();
	SendCommand(0x12);//soft  reset
	WaitUntilIdle();
	/*	Y increment, X increment	*/
	SendCommand(0x11);
	SendData(0x03);
	/*	Set RamX-address Start/End position	*/
	SendCommand(0x44);
	SendData(0x00);	
	SendData(((width-1) >> 3) & 0x1f);
	/*	Set RamY-address Start/End position	*/
	SendCommand(0x45);
	SendData(0);
	SendData(0);
	SendData((height&0xff));
	SendData((height&0x100)>>8);

	SendCommand(0x21); //  Display update control
	SendData(0x00);
	SendData(0x80);	

    SendCommand(0x4E); // SET_RAM_X_ADDRESS_COUNTER
    SendData(0x00);
    SendCommand(0x4F); // SET_RAM_Y_ADDRESS_COUNTER
    SendData(0x00);
    SendData(0x00);
	
	WaitUntilIdle();
	return 0;
}

/**
 *  @brief: basic function for sending commands
 */
void Epd::SendCommand(unsigned char command) {
    DigitalWrite(dc_pin, LOW);
    SpiTransfer(command);
}

/**
 *  @brief: basic function for sending data
 */
void Epd::SendData(unsigned char data) {
    DigitalWrite(dc_pin, HIGH);
    SpiTransfer(data);
}

/**
 *  @brief: Wait until the busy_pin goes HIGH
 */
void Epd::WaitUntilIdle(void) {
	Serial.print("e-Paper busy \r\n ");
	UBYTE busy;
	// BUSY is active HIGH on this SSD1680 panel - the opposite of the UC8253
	// 3.52" panel in this repo. Do not "fix" the polarity to match that driver.
	UDOUBLE start = millis();
	do {
		// A full refresh on this panel is ~13s; 40s means BUSY is never coming
		// back. Bail out loudly instead of hanging forever, so a stall is
		// distinguishable from a slow refresh.
		if (millis() - start > 40000) {
			Serial.print("e-Paper BUSY timeout! \r\n ");
			return;
		}
		DelayMs(20);
		busy = DigitalRead(busy_pin);
	} while(busy);
	DelayMs(20);
	Serial.print("e-Paper busy release \r\n ");
}

/******************************************************************************
function :	Turn On Display
parameter:
******************************************************************************/
void Epd::TurnOnDisplay(void)
{
    SendCommand(0x20);
    WaitUntilIdle();
}

/******************************************************************************
function :  Display Array data
******************************************************************************/

// The bundled demo bitmaps are authored for the orientation Waveshare's own
// driver board gives this panel. Seated in a 24-pin FPC connector - the Seeed
// v2 board's or the ThinkInk one's, it makes no difference, this is a panel
// fact - the raster comes out 180 degrees round, the Waveshare logo bottom-right
// instead of top-left. Rotated here rather than by re-authoring imagedata.cpp,
// so the vendor's arrays stay byte-for-byte theirs.
//
// A 180 degree rotation of a byte-aligned raster is just the buffer walked
// backwards with each byte's bits mirrored; 152 px is exactly 19 bytes, so no
// sub-byte shift is needed and this stays exact. Set to 0 for Waveshare's
// original orientation.
#define EPD_2IN66B_ROTATE_180   1

static UBYTE reverse_bits(UBYTE b) {
    b = (b >> 4) | (b << 4);
    b = ((b & 0xCC) >> 2) | ((b & 0x33) << 2);
    b = ((b & 0xAA) >> 1) | ((b & 0x55) << 1);
    return b;
}

static UBYTE plane_byte(const UBYTE *buf, UWORD i, UWORD j, UWORD Width,
                        UWORD Height) {
#if EPD_2IN66B_ROTATE_180
    return reverse_bits(pgm_read_byte(&buf[(Width - 1 - i) + (Height - 1 - j) * Width]));
#else
    return pgm_read_byte(&buf[i + j * Width]);
#endif
}

void Epd::DisplayFrame(const UBYTE *Image, const UBYTE *ImageRed) {
    UWORD Width, Height;
    Width = (width % 8 == 0)? (width / 8 ): (width / 8 + 1);
    Height = height;

    SendCommand(0x24);
    for (UWORD j = 0; j <Height; j++) {
        for (UWORD i = 0; i <Width; i++) {
            SendData(plane_byte(Image, i, j, Width, Height));
        }
    }
    SendCommand(0x26);
    for (UWORD j = 0; j <Height; j++) {
        for (UWORD i = 0; i <Width; i++) {
            SendData(~plane_byte(ImageRed, i, j, Width, Height));
        }
    }
    TurnOnDisplay();
}

/******************************************************************************
function :  Clear Screen
parameter:
  mode: 0:just partial mode
        1:clear all
******************************************************************************/
void Epd::Clear(void) {
    UWORD Width, Height;
    Width = (width % 8 == 0)? (width / 8 ): (width / 8 + 1);
    Height = height;
    // Waveshare's loops run j <= Height, pushing one row (19 bytes) past the RAM
    // window and wrapping the address counter back over row 0. Bounded properly.
    SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            SendData(0xff);
        }
    }
    SendCommand(0x26);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            SendData(0x00);
        }
    }
    TurnOnDisplay();
}

/**
 *  @brief: After this command is transmitted, the chip would enter the 
 *          deep-sleep mode to save power. 
 *          The deep sleep mode would return to standby by hardware reset. 
 *          The only one parameter is a check code, the command would be
 *          You can use EPD_Reset() to awaken
 */
void Epd::Sleep(void) {
    // Left as Waveshare wrote it. The Waveshare_3in52 port in this repo sends
    // POWER_OFF (0x02) before deep sleep, but that is a UC8253 command; SSD1680
    // has no equivalent and enters deep sleep directly from 0x10.
    SendCommand(0X10);
    SendData(0x01);
}



/* END OF FILE */
