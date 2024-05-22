/*! ***************************************************************************
 *
 * \brief     Low level driver for TPM1
 * \file      tpm1.c
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
#include "tpm1.h"

/*!
 * \brief Initialises Timer/PWM Module 1 (TPM1)
 */
void tpm1_init(void)
{
	// Enable TPM1 clock
	SIM->SCGC6 |= SIM_SCGC6_TPM1(1);
	
	// Enable PORTB clock
	SIM->SCGC5 |= SIM_SCGC5_PORTB(1);
	
	// Select ALT3 -> TPM1_CH1
	PORTB->PCR[1] = PORT_PCR_MUX(3);
	
	// Set the prescaler to 16 of TPM1. The TPM1 is now clocked with a frequency of 48 MHz / 16 = 3 MHz
	TPM1->SC |= TPM_SC_PS(4);
	
	// MOD setup: 3MHz / 50Hz = 6000
	TPM1->MOD = 60000-1;
	
	// Edge aligned and high true PWM
	TPM1->CONTROLS[1].CnSC = TPM_CnSC_MSB(1) | TPM_CnSC_ELSB(1);
	
	// Set the initial compare match value to 1.5 ms. MOD = 60000. 60000/20=3000 counts per milisecond. 1.5ms = 4500 counts
	TPM1->CONTROLS[1].CnV = 4500;
	
	// Enable TPM1
	TPM1->SC |= TPM_SC_CMOD(1);
}
