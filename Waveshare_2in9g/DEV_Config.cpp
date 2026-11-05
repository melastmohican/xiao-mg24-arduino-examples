/*****************************************************************************
* | File      	:   DEV_Config.c
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2020-02-19
* | Info        :
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "DEV_Config.h"

void GPIO_Config(void)
{
    pinMode(EPD_BUSY_PIN,  INPUT);
    pinMode(EPD_RST_PIN , OUTPUT);
    pinMode(EPD_DC_PIN  , OUTPUT);
    pinMode(EPD_CS_PIN , OUTPUT);

    digitalWrite(EPD_CS_PIN , HIGH);
}

void GPIO_Mode(UWORD GPIO_Pin, UWORD Mode)
{
    if(Mode == 0) {
        pinMode(GPIO_Pin , INPUT);
	} else {
		pinMode(GPIO_Pin , OUTPUT);
	}
}
/******************************************************************************
function:	Module Initialize, the BCM2835 library and initialize the pins, SPI protocol
parameter:
Info:
******************************************************************************/
UBYTE DEV_Module_Init(void)
{
	//gpio
	GPIO_Config();

	//serial
	Serial.begin(115200);
	// Serial on the XIAO MG24 is EUSART0 bridged through the on-board CMSIS-DAP
	// chip, not a USB CDC endpoint, so bound the wait - it must never block.
	while (!Serial && millis() < 3000) {
		delay(10);
	}

	// spi: the EPD signals are already on the default SPI (D8=SCK, D10=MOSI),
	// so there is nothing to remap. The transaction is opened once and left
	// open for the lifetime of the sketch - see DEV_SPI_Init() below.
	EPD_SPI_PORT.begin();
	EPD_SPI_PORT.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));

	return 0;
}


// Hands SCK/MOSI back to the GPIO block for the bit-banged DEV_SPI_SendByte /
// DEV_SPI_ReadByte helpers below. Unused by the 3.52B driver, which only ever
// calls DEV_SPI_WriteByte.
void DEV_GPIO_Init(void)
{
    EPD_SPI_PORT.end();
    pinMode(EPD_SCK_PIN, OUTPUT);
    pinMode(EPD_MOSI_PIN, OUTPUT);
}

// Counterpart to DEV_GPIO_Init: re-takes the pins for the SPI peripheral.
// Deliberately does NOT call beginTransaction. On the Silicon Labs core
// SilabsSPI::beginTransaction() takes a FreeRTOS mutex with portMAX_DELAY and,
// when the settings are unchanged, returns while still holding it - so a second
// call without an intervening endTransaction() deadlocks. DEV_Module_Init()
// already opened the one transaction this sketch needs.
void DEV_SPI_Init(void)
{
    EPD_SPI_PORT.begin();
}


/******************************************************************************
function:
			SPI read and write
******************************************************************************/
void DEV_SPI_WriteByte(UBYTE data)
{
    EPD_SPI_PORT.transfer(data);
}

void DEV_SPI_Write_nByte(UBYTE *pData, UDOUBLE len)
{
    for (int i = 0; i < len; i++)
        DEV_SPI_WriteByte(pData[i]);
}

void DEV_SPI_SendByte(UBYTE data)
{
    GPIO_Mode(EPD_MOSI_PIN, OUTPUT);
    digitalWrite(EPD_CS_PIN, GPIO_PIN_RESET);
    for (int i = 0; i < 8; i++)
    {
        if ((data & 0x80) == 0) digitalWrite(EPD_MOSI_PIN, GPIO_PIN_RESET); 
        else                    digitalWrite(EPD_MOSI_PIN, GPIO_PIN_SET);

        data <<= 1;
        digitalWrite(EPD_SCK_PIN, GPIO_PIN_SET);     
        digitalWrite(EPD_SCK_PIN, GPIO_PIN_RESET);
    }
    digitalWrite(EPD_CS_PIN, GPIO_PIN_SET);
}

UBYTE DEV_SPI_ReadByte()
{
    UBYTE j=0xff;
    GPIO_Mode(EPD_MOSI_PIN, INPUT);
    digitalWrite(EPD_CS_PIN, GPIO_PIN_RESET);
    for (int i = 0; i < 8; i++)
    {
        j = j << 1;
        if (digitalRead(EPD_MOSI_PIN))  j = j | 0x01;
        else                            j = j & 0xfe;
        
        digitalWrite(EPD_SCK_PIN, GPIO_PIN_SET);     
        digitalWrite(EPD_SCK_PIN, GPIO_PIN_RESET);
    }
    digitalWrite(EPD_CS_PIN, GPIO_PIN_SET);
    GPIO_Mode(EPD_MOSI_PIN, 1);
    return j;
}




void DEV_Module_Exit(void)
{
    // Nothing to do on the Seeed ePaper Driver Board v2: it has no software
    // power gate. The panel is already in deep sleep by this point
    // (EPD_3IN52B_sleep), which is as low-power as it gets here.
}

