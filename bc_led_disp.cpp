/******************************************************************************
*  bc_led_disp.cpp
*  BC759x 7 segment LED display Driver Library
*  This library can be used with the following chips:
*    BC7595  --  48 segments LED display with 48-key keyboard interface
*    BC7591  --  256 segments LED display with 96-key keyboard interface
*
*  Dependencies:
*     This Library depends on the Arduino Serial or Software Serial library.
*
*  Author:
*     This library is written by BitCode. https://bitcode.com.cn
*
*  Version:
*     V1.0 March 2021
*     V1.1 May 2021, fixed bug in decimal display
*     V1.2 May 2021, added 'keep leading 0s' feature in displayDec()
*     V2.0 July 2021, changed to use Stream class as base class.
*
*  License:
*     MIT license. It can be used for both open source and commercial projects.
******************************************************************************/
#include "bc_led_disp.h"

const uint8_t BlinkBit[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

/******************************************************************************
* Constructor
* This class can also be initialized by hardware or software serial. If software
* serial is used, it would be user's responsibility to create an instance of
* the software serial port and using it as a parameter of BcLedDisp class
* constructor. And user must initialize the software serial port by calling
* begin(9600) before using any of the library functions.
******************************************************************************/
BcLedDisp::BcLedDisp(Stream& SerialPort) : Uart(SerialPort)
{
}

/******************************************************************************
* Set Display Direction Low DIG number on right
* The PCB has the lower DIG number on right. This is the default setting.
* This setting will affect the output of the dispHex() and dispDec() functions.
* Input Parameters: none
******************************************************************************/
void BcLedDisp::setDispLowDigOnRight()
{
    LowDigOnRight = true;
}

/******************************************************************************
* Set Display Direction Low DIG number on left
* The PCB has the higher DIG number on right.
* This setting will affect the output of the dispHex() and dispDec() functions.
* Input Parameters: none
******************************************************************************/
void BcLedDisp::setDispLowDigOnLeft()
{
    LowDigOnRight = false;
}

/******************************************************************************
* Clear Display
* This is a wrapper of sending WRITE_ALL command with 0x00 as data, and set all
* the blink controls to non-blinking.
* Input Parameters: none
* Return Value: none
******************************************************************************/
void BcLedDisp::clear()
{
    sendCmd(WRITE_ALL, 0x00);
    for (uint8_t i = 0; i < 0x10; i++)
    {
        sendCmd(BLINK_WT_CLR | i, 0xff);
    }
    sendCmd(BLINK_DIG_CTL, 0x00);
    sendCmd(BLINK_DIG_CTL + 1, 0x00);
    BlinkCtlH = 0;
    BlinkCtlL = 0;
}

/******************************************************************************
* Send Command 
* Using this function to send any command to BC759x LED display drivers.
* Input Parameters:
*     uint8_t Cmd   - The command to be be sent
*     uint8_t Data  - Data to be sent
* Return Value: none
******************************************************************************/
void BcLedDisp::sendCmd(uint8_t Cmd, uint8_t Data)
{
	Uart.write(Cmd);
	Uart.write(Data);
}

/******************************************************************************
* Display Decimal Number
* A high level function to display decimal numbers. Only unsigned value can be
* displayed.
* Input Parameters:
*     uint32_t Val  - The Value to be displayed. Range from 0 to 4,294,967,295
*     uint8_t Pos   - The digit number where the least significant digit will
*                     be displayed.
*     uint8_t Width - Display width, it can be larger or smaller than the actual
*                     width of Val. Only lower 7 bits are used as width setting,
*                     the bit7 is used as indicator of whether leading 0s will
*                     be displayed if Width is larger than Val's actual width.
******************************************************************************/
void BcLedDisp::displayDec(uint32_t Val, uint8_t Pos, uint8_t Width)
{
    if ((Pos > 31) || ((Width & 0x7f) == 0))
    {
        return;
    }
    sendCmd(DECODE_WT | Pos, Val % 10);
    Val = Val / 10;
    if (LowDigOnRight)
    {
        if (++Pos > 31)
        {
            return;
        }
        while ((--Width & 0x7f) > 0)
        {
            if ((Val >= 10) || (Width & 0x80))
            {
                sendCmd(DECODE_WT | Pos, Val % 10);
            }
            else
            {
                sendCmd(DECODE_WT | Pos, (Val % 10) | 0x80);
            }
            Val = Val / 10;
            if (++Pos > 31)
            {
                return;
            }
        }
    }
    else
    {
        if (Pos-- == 0)
        {
            return;
        }
        while ((--Width & 0x7f) > 0)
        {
            if ((Val >= 10) || (Width & 0x80))
            {
                sendCmd(DECODE_WT | Pos--, Val % 10);
            }
            else
            {
                sendCmd(DECODE_WT | Pos--, (Val % 10) | 0x80);
            }
            Val = Val / 10;
            if (Pos == 0)
            {
                return;
            }
        }
    }
}

/******************************************************************************
* Display Hexdecimal Number
* A high level function to display hexdecimal numbers. A maximum 16-bit value
* will be accepted as input, but larger numbers are easily splited and
* displayed by calling this function multiply.
* Input Parameters:
*     uint16_t Val  - The Value to be displayed. Range from 0 to 0xffff
*     uint8_t Pos   - The DIG line number where the least significant digit will
*                     be displayed.
*     uint8_t Width - How many digits the Val will be displayed as. It can be
*                     less or more than the actually width of the Val, if 
*                     Width is larger than the actual width of the input value,
*                     0s will be padded in front of it.
******************************************************************************/
void BcLedDisp::displayHex(uint16_t Val, uint8_t Pos, uint8_t Width)
{
    uint8_t i;
    if (Pos > 31)
    {
        return;
    }
    if (LowDigOnRight)
    {
        if (Width + Pos > 32)
        {
            Width = 32 - Pos;
        }
        for (i = Pos; i < Width + Pos; i++)
        {
            sendCmd(DECODE_WT | i, Val & 0x0f);
            Val >>= 4;
        }
    }
    else
    {
        if (Width > Pos + 1)
        {
            Width = Pos + 1;
        }
        for (i = 0; i < Width; i++)
        {
            sendCmd(DECODE_WT | (Pos - i), Val & 0x0f);
            Val >>= 4;
        }
    }
}

void BcLedDisp::digitBlink(uint8_t Digit, bool OnOff)
{
    if (Digit > 31)
    {
        return;
    }
    if (Digit < 16)
    {
        if (OnOff)
        {
            sendCmd(BLINK_WT_SET | Digit, 0xff);
        }
        else
        {
            sendCmd(BLINK_WT_CLR | Digit, 0xff);
        }
    }
    else
    {
        if (Digit & 0x08)
        {
            BlinkCtlH = OnOff ? BlinkCtlH | BlinkBit[Digit & 0x07] : BlinkCtlH & (~BlinkBit[Digit & 0x07]);
            sendCmd(BLINK_DIG_CTL + 1, BlinkCtlH);
        }
        else
        {
            BlinkCtlL = OnOff ? BlinkCtlL | BlinkBit[Digit & 0x07] : BlinkCtlL & (~BlinkBit[Digit & 0x07]);
            sendCmd(BLINK_DIG_CTL, BlinkCtlL);
        }
    }
}
