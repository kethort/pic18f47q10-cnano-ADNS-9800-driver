//=============================================================================
//
// MIT License
// 
// Copyright (c) 2021 Grzegorz Pietrusiak
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// 
// Project name: Amiga Laser Mouse ADNS-9800
// Project description: based on PIC18f23k22 microcontroller and ADNS-9800 Laser Gaming Sensor
// PCB for the project: https://www.pcbway.com/project/shareproject/Amiga_Laser_Mouse.html
// Toolchain: SDCC 3.9.0-rc1
// 	gputils-1.5.0-1
//
// Author: Grzegorz Pietrusiak
// Email: gpsspam2@gmail.com
// 
//=============================================================================

#include "eeprom.h"
#include "mcc_generated_files/mcc.h"

//=============================================================================
bool EE_write_byte(uint8_t u8AddressP, uint8_t u8DataP)
{
    bool bWriteStatus = true;
    EEDATA = u8DataP;
    EEADR = u8AddressP;
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS = 0;
    EECON1bits.WREN = 1;
    INTCONbits.GIE = 0;
    EECON2 = 0x55;
    EECON2 = 0x0AA;
    EECON1bits.WR = 1;
    INTCONbits.GIE = 1;
    while (EECON1bits.WR);
    if (EECON1bits.WRERR)
    {
        bWriteStatus = false;
    }
    EECON1bits.WREN = 0;
    return bWriteStatus;
}

//=============================================================================
uint8_t EE_read_byte(uint8_t u8AddressP)
{
    EEADR = u8AddressP;
    EECON1bits.CFGS = 0;
    EECON1bits.EEPGD = 0;
    EECON1bits.RD = 1;
    return EEDATA;
}

//=============================================================================
