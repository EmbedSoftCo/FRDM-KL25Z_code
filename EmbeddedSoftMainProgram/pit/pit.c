/*! ***************************************************************************
 *
 * \brief     Low level driver for the PIT
 * \file      pit.c
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
#include "pit.h"

volatile bool displayFlag;
volatile bool logFlag;
volatile bool gpsFlag;

/*!
 * \brief Initialises the Periodic Interrupt Timer (PIT)
 */
void pit_init(void)
{
	// Enable PIT module
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	
	// Enable module, freeze timers in debug mode
	PIT->MCR &= ~PIT_MCR_MDIS_MASK;
	PIT->MCR |= PIT_MCR_FRZ_MASK;
	
	// Initialize PITO to generate an event every 1 seconds
	PIT->CHANNEL[0].LDVAL = PIT_LDVAL_TSV(24e6-1);
	
	// Initialize PIT1 to generate an event every 15 seconds
	PIT->CHANNEL[1].LDVAL = PIT_LDVAL_TSV((1440e6/4)-1);
	
	// No chaining
	PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_CHN_MASK; 
	PIT->CHANNEL[1].TCTRL &= ~PIT_TCTRL_CHN_MASK; 
	
	// Generate interrupts
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TIE_MASK;
	
	// Enable interrupts
	NVIC_SetPriority(PIT_IRQn, 1);
	NVIC_ClearPendingIRQ(PIT_IRQn);
	NVIC_EnableIRQ(PIT_IRQn);
	
	// Enable counter
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TEN_MASK;
}

void PIT_IRQHandler(void)
{		
		static uint16_t cnt = 0;
	
		// Clear pending IRQ
		NVIC_ClearPendingIRQ(PIT_IRQn);
		
		// PIT0 is for LED
		if(PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK)
		{
			//Clear flag
			PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;
			cnt++;
			gpsFlag = true; //update every second
			if(cnt == 9)
			{
				cnt = 0;
				displayFlag = true; //update every 10 seconds
			}
		}			

		// PIT1 used for logging
		if(PIT->CHANNEL[1].TFLG & PIT_TFLG_TIF_MASK)
		{
			// Clear flag
			PIT->CHANNEL[1].TFLG |= PIT_TFLG_TIF_MASK;
			logFlag = true;
		}
		
}
