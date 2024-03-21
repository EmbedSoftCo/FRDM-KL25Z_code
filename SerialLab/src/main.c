/*! ***************************************************************************
 *
 * \brief     SerialLab project
 * \file      main.c
 * \author    Hugo Arends
 * \date      July 2021
 *
 * \copyright 2021 HAN University of Applied Sciences. All Rights Reserved.
 *            \n\n
 *            Permission is hereby granted, free of charge, to any person
 *            obtaining a copy of this software and associated documentation
 *            files (the "Software"), to deal in the Software without
 *            restriction, including without limitation the rights to use,
 *            copy, modify, merge, publish, distribute, sublicense, and/or sell
 *            copies of the Software, and to permit persons to whom the
 *            Software is furnished to do so, subject to the following
 *            conditions:
 *            \n\n
 *            The above copyright notice and this permission notice shall be
 *            included in all copies or substantial portions of the Software.
 *            \n\n
 *            THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *            EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *            OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *            NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *            HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *            WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *            FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *            OTHER DEALINGS IN THE SOFTWARE.
 *
 *****************************************************************************/
#include <MKL25Z4.h>
#include <stdio.h>

#include "rgb.h"
#include "uart0.h"
#include "uart2.h"

// Global variales
volatile static uint32_t cnt = 0;

// Local function prototypes
static void delay_us(uint32_t d);

/*!
 * \brief Main application
 */
int main(void) {
    rgb_init();
    uart0_init();

    uart0_send_string("Serial Lab\r\n");

    while (1) {
        uint8_t data = uart2_receive_poll();
        uart0_put_char(data);
        // todo put data into a buffer;
        // todo parsebuffer with nmea_single_parse()
    }
}

/*!
 * \brief Creates a blocking delay
 *
 * Software delay of approximately 1.02 us, depending on compiler version,
 * CPU clock frequency and optimizations.
 * - C compiler: ARMClang v6
 * - Language C: gnu11
 * - CPU clock: 48 MHz
 * - Optimization level: -O3
 * - Link-Time Optimization: disabled
 *
 * \param[in]  d  delay in microseconds
 */
static void delay_us(uint32_t d) {
#if (CLOCK_SETUP != 1)
#warning This delay function does not work as designed
#endif

    volatile uint32_t t;

    for (t = 4 * d; t > 0; t--) {
        __asm("nop");
        __asm("nop");
    }
}
