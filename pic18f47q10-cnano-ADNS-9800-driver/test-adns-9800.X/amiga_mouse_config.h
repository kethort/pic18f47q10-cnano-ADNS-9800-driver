#ifndef __AMIGA_MOUSE_CONFIG_H__
#define __AMIGA_MOUSE_CONFIG_H__
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
// Includes
//=============================================================================
#include "mcc_generated_files/mcc.h"
#include <stdint.h>

//=============================================================================
// Microcontroller Input and Output ports
//=============================================================================

#define NCS  (LATBbits.LATB5)
#define MISO (PORTDbits.RD7)
#define MOSI (LATDbits.LATD6)
#define SCLK (LATDbits.LATD5)

#define NCS_PORT_DIRECTION  (TRISBbits.TRISB5)
#define MISO_PORT_DIRECTION (TRISDbits.TRISD7)
#define MOSI_PORT_DIRECTION (TRISDbits.TRISD6)
#define SCLK_PORT_DIRECTION (TRISDbits.TRISD5)

#endif // __AMIGA_MOUSE_CONFIG_H__
