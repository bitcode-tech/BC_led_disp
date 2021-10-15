#ifndef BC_LED_DISP_H
#define BC_LED_DISP_H

/******************************************************************************
*  bc_led_disp.h
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
#include <Arduino.h>
#include <Stream.h>

/***** BC759x commands *****/
#define DIRECT_WT     (0x00)
#define COL_WRITE     (0x00)
#define BLINK_WT_CLR  (0x20)
#define BLINK_WT_SET  (0x30)
#define SHIFT_H_WT    (0x40)
#define ROTATE_R      (0x5f)
#define ROTATE_L      (0x60)
#define SHIFT_L_WT    (0x61)
#define DECODE_WT     (0x80)
#define QTR_WT_BOT    (0xa0)
#define QTR_INS_BOT   (0xa4)
#define QTR_WT_TOP    (0xa8)
#define WRITE_EXT     (0xa8)
#define QTR_INS_TOP   (0xac)
#define DECODE_EXT    (0xb0)
#define SEG_OFF       (0xc0)
#define COORD_OFF     (0xc0)
#define SEG_ON        (0xc1)
#define COORD_ON      (0xc1)
#define BLINK_DIG_CTL (0xd0)
#define GLOBAL_CTL    (0xf0)
#define WRITE_ALL     (0xf1)
#define BLINK_SPEED   (0xf2)
#define DIM_CTL       (0xf3)
#define RESET_H       (0xff)
#define RESET_L       (0x5a)
#define UART_SEND_0_H (0xff)
#define UART_SEND_0_L (0xff)

class BcLedDisp
{
public:
    /******************************************************************************
* Constructors
* This class can be initialized by hardware serial or software serial
******************************************************************************/
    BcLedDisp(Stream& SerialPort);

    /******************************************************************************
* Send Command 
* Using this function to send any command to BC759x LED display drivers.
* Input Parameters:
*     uint8_t Cmd   - The command to be be sent
*     uint8_t Data  - Data to be sent
* Return Value: none
******************************************************************************/
    void sendCmd(uint8_t Cmd, uint8_t Data);

    /******************************************************************************
* Clear Display
* This is a wrapper of sending WRITE_ALL command with 0x00 as data, and set all
* the blink controls to non-blinking.
* Input Parameters: none
* Return Value: none
******************************************************************************/
    void clear();

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
    void displayDec(uint32_t Val, uint8_t Pos, uint8_t Width);

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
    void displayHex(uint16_t Val, uint8_t Pos, uint8_t Width);

    /******************************************************************************
* Setting The Blink Property Of A Digit
* This is a high level wrapper of the operation of blink control commands. 
* User can control the blink property of every individual digits. The library
* will keep track of the status of each digit ( only for the status set via
* this function, changing the blink status via direct command will not be
* acknowledged by the library.
* Input Parameters:
*     uint8_t Digit  - The digit number to be affected by this operation, range
*                      from 0 to 31.
*     bool OnOff     - The blink status to be set to. 0=non-blinking, 1= blinking
* Return Value: none
******************************************************************************/
    void digitBlink(uint8_t Digit, bool OnOff);

    /******************************************************************************
* Set Display Direction Low DIG number on right
* The PCB has the lower DIG number on right. This is the default setting.
* This setting will affect the output of the dispHex() and dispDec() functions.
* Input Parameters: none
******************************************************************************/
    void setDispLowDigOnRight();

    /******************************************************************************
* Set Display Direction Low DIG number on left
* The PCB has the higher DIG number on right.
* This setting will affect the output of the dispHex() and dispDec() functions.
* Input Parameters: none
******************************************************************************/
    void setDispLowDigOnLeft();

private:
    bool            UsingHwSerial;
    bool            LowDigOnRight = true;
    uint8_t         BlinkCtlL;
    uint8_t         BlinkCtlH;
    Stream&			Uart;
};

#endif
