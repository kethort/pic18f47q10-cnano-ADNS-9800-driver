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

#include <string.h>
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/drivers/uart.h"
#include "amiga_mouse_config.h"
#include "adns9800.h"
#include "spi.h"
#include <stdbool.h>

//=============================================================================
// Global variables
//=============================================================================
bool g_bAdnsEnabled = false;

static inline void ADNS_init(void) {
    ADNS_com_end();
    ADNS_com_begin();
    ADNS_com_end(); // ensure that the serial port is reset
    ADNS_write_reg(REG_Power_Up_Reset, 0x5a);
    __delay_ms(50); // 50ms power up time
    
    // read registers 0x02 to 0x06 (and discard the data)
    (void)ADNS_read_reg(REG_Motion);
    (void)ADNS_read_reg(REG_Delta_X_L);
    (void)ADNS_read_reg(REG_Delta_X_H);
    (void)ADNS_read_reg(REG_Delta_Y_L);
    (void)ADNS_read_reg(REG_Delta_Y_H);
    
    // upload the firmware
    printf("ADNS9800 Uploading firmware...\n");
    ADNS_upload_firmware();
    __delay_ms(10);
    
    // check firmware correctness
    uint8_t u8ProductId = ADNS_read_reg(REG_Product_ID);
    
    if (ADNS_SUPPORTED_PRODUCT_ID == u8ProductId) {
        // Test communication with ADNS-9800 by checking if Product ID matches Inverse Product ID
        if (0xff == (ADNS_read_reg(REG_Inverse_Product_ID) ^ u8ProductId)) {
            // SROM CRC test
            ADNS_write_reg(REG_SROM_Enable, 0x15); 
            __delay_ms(10);
            
            uint8_t u8CrcLow = ADNS_read_reg(REG_Data_Out_Lower);
            uint8_t u8CrcHigh = ADNS_read_reg(REG_Data_Out_Upper);
            
            if ((0xEF == u8CrcLow) && (0xBE == u8CrcHigh)) {
                g_bAdnsEnabled = true;
                // enable laser(bit 0 = 0b), in normal mode (bits 3,2,1 = 000b)
                // reading the actual value of the register is important because the real
                // default value is different from what is said in the datasheet, and if you
                // change the reserved bits (like by writing 0x00...) it would not work.
                uint8_t u8LaserDriveMode = ADNS_read_reg(REG_LASER_CTRL0);
                ADNS_write_reg(REG_LASER_CTRL0, u8LaserDriveMode & 0xf0 );
                __delay_ms(1);

                printf("Optical Chip Initialized\n");
            }
            else {
                printf("SROM CRC error: 0x%X\n", ((u8CrcHigh << 8) | u8CrcLow));
            }
        }
        else {
            printf("Product ID test failed\n");
        }
    }
    else {
        printf("Invalid Product ID: 0x%X\n", u8ProductId);
    }
}

void ADNS_uart_print_register(uint8_t u8RegIdP, char * szRegNameP) {
    uint8_t u8RegValue = ADNS_read_reg(u8RegIdP);
    printf("%s : 0x%X\n", szRegNameP, u8RegValue);
}

static inline void ADNS_dispRegisters(void) {
    ADNS_uart_print_register(REG_Product_ID, "Product ID");
    ADNS_uart_print_register(REG_Revision_ID, "Revision ID");
    ADNS_uart_print_register(REG_Motion, "Motion");
    ADNS_uart_print_register(REG_Observation, "Observation");
    ADNS_uart_print_register(REG_SROM_ID, "SROM ID");
    ADNS_uart_print_register(REG_Configuration_I, "Config I");
    ADNS_uart_print_register(REG_Configuration_II, "Config II");
    ADNS_uart_print_register(REG_Configuration_IV, "Config IV");
    ADNS_uart_print_register(REG_Configuration_V, "Config V");
}

void main(void) {
    SYSTEM_Initialize();
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    
    ANSELB = 0; // Disable all analog inputs on port B
    ANSELD = 0; // Disable all analog inputs on port D  
    
    SPI_init();
    ADNS_init();
    ADNS_dispRegisters();
    
    while (1) {
        if (g_AdnsMotion) {
            g_AdnsMotion = 0;
            static int16_t i16DeltaX = 0;
            static int16_t i16DeltaY = 0;

            if (g_bAdnsEnabled) {
                // handle mouse X and Y position
                motion_t motion;
                *((uint8_t *)&motion) = ADNS_read_reg(REG_Motion);

                // check if no fault occurred
                if (motion.LP_VALID && !motion.FAULT) {
                    if (motion.MOT) { // if movement occurred
                        // registers must be read in the order: REG_Delta_X_L first, then REG_Delta_X_H
                        i16DeltaX = (uint16_t)ADNS_read_reg(REG_Delta_X_L); 
                        i16DeltaX |= ((uint16_t)ADNS_read_reg(REG_Delta_X_H) << 8);
                        
                        // registers must be read in the order: REG_Delta_Y_L first, then REG_Delta_Y_H
                        i16DeltaY = (uint16_t)ADNS_read_reg(REG_Delta_Y_L); 
                        i16DeltaY |= ((uint16_t)ADNS_read_reg(REG_Delta_Y_H) << 8);

                        printf("motion=(%d, %d)\n", i16DeltaX, i16DeltaY);
                    }
                }
                else {
                    printf("Error: motion=0x%X\n", *((uint8_t *)&motion));
                }
            }
        }
    }
}