/**
 *  @filename   :   epdif.h
 *  @brief      :   Header file of epdif.cpp providing EPD interface functions
 *                  Users have to implement all the functions in epdif.cpp
 *  @author     :   Yehui from Waveshare
 *
 *  Copyright (C) Waveshare     August 10 2017
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

#ifndef EPDIF_H
#define EPDIF_H

#include <Arduino.h>
#include <SPI.h>

// Pin definition
// Seeed Studio ePaper Driver Board for XIAO v2: the panel sits in the board's
// 24-pin FPC connector and the board hard-wires the EPD signals to these XIAO
// pins. Same mapping as the GxEPD2 demos and Waveshare_3in52 in this repo.
//   https://wiki.seeedstudio.com/xiao_eink_expansion_board_v2/
#define RST_PIN         D0
#define DC_PIN          D3
#define CS_PIN          D1
#define BUSY_PIN        D2
#define EPD_SCK_PIN     D8    // PA3, the MG24's hardware SPI SCK
#define EPD_MOSI_PIN    D10   // PA5, the MG24's hardware SPI MOSI

// Default SPI already lands on D8/D10 (see the core's variants/xiao_mg24/
// pins_arduino.h), so there is nothing to remap - and the Silicon Labs core has
// no setSCK()/setTX() equivalent anyway.
#define EPD_SPI_PORT    SPI

// No PWR_PIN: the 24-pin connector has no software power gate, so there is
// nothing to switch on before talking to the panel.

class EpdIf {
public:
    EpdIf(void);
    ~EpdIf(void);

    static int  IfInit(void);
    static void DigitalWrite(int pin, int value); 
    static int  DigitalRead(int pin);
    static void DelayMs(unsigned int delaytime);
    static void SpiTransfer(unsigned char data);
};

#endif
