/******************************************************************************
*  led_disp_example.ino
*  BC759x 7 segment LED display Driver Library Example Code
*
*  This code uses SofewareSerial to drive a BC7595 or BC7591 to display a 3
*  digits counter on DIG0-2 which will increase at an interval of 10ms.
*  The SoftwareSerial uses digital I/O pin 2 as Rx and pin 3 as Tx.
*  The SoftwareSerial can be replaced by hardware serial such as Serial,
*  Serial1, or Serial2.
*  This code runs on any Arduino compatible boards.
*
*  Dependencies:
*     This code depends on the following libraries:
*        Arduino Software Serial Library
*        BitCode BC_led_disp Library
*
*  Author:
*     This software is written by BitCode. https://bitcode.com.cn
*
*  Version:
*     V1.0 March 2021
*
*  License:
*     MIT license. It can be used for both open source and commercial projects.
******************************************************************************/
#include <bc_led_disp.h>

#include <SoftwareSerial.h>

SoftwareSerial swSerial(2, 3);        // creating SoftwareSerial instance, using pin 2 as Rx, 3 as Tx
BcLedDisp      Disp(swSerial);        // creating BcLedDisp instance using software serial port swSerial

uint16_t cnt;

void setup()
{
    swSerial.begin(9600);        // Initializing swSerial
    Disp.clear();                // Clear any display contents
}

void loop()
{
    // put your main code here, to run repeatedly:
    Disp.displayDec(cnt, 0, 3);        // Display cnt as decimal number on DIG0-DIG2 (3 digits wide)
    cnt = cnt + 1;
    if (cnt == 1000)
    {
        cnt = 0;
    }
    delay(10);
}
