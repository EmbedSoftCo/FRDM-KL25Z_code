/*! ***************************************************************************
 *
 * \brief     Low level driver for uart2
 * \file      uart2.c
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
#include "uart2.h"

void uart2_init(void)
{
    uint32_t divisor;
    
    // Enable clock to UART and port E
    SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
    
    // Connect UART to pins for PTE22, PTE23
    PORTE->PCR[22] = PORT_PCR_MUX(4);
    PORTE->PCR[23] = PORT_PCR_MUX(4);
    
    // Ensure tx and rx are disabled before configuration
    UART2->C2 &= ~(UARTLP_C2_TE_MASK | UARTLP_C2_RE_MASK);
    
    // Set baud rate
    divisor = 24000000/(9600*16);
    UART2->BDH = UART_BDH_SBR(divisor>>8);
    UART2->BDL = UART_BDL_SBR(divisor);
    
    // No parity, 8 data bits, one stop bit
    UART2->C1 = UART2->S2 = UART2->C3= 0;

     // Enable transmitter and reciever
    UART2->C2 = UART_C2_TE_MASK | UART_C2_RE_MASK;
}

void uart2_transmit_poll(uint8_t data)
{
    // Wait until transmit data register is empty
    while (!(UART2->S1 & UART_S1_TDRE_MASK))
    {}
    
    UART2->D = data;
}

uint8_t uart2_receive_poll(void)
{
    // Wait until receive data register is full
    while (!(UART2->S1 & UART_S1_RDRF_MASK))
    {}

    return UART2->D;
}
