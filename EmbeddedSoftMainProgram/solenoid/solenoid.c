/*! ***************************************************************************
 *
 * \brief     Low level driver for the LEDs
 * \file      leds.c
 * \author    Hugo Arends
 * \date      April 2021
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
#include "solenoid.h"

/*!
 * \brief Initialises the solenoid
 *
 * This functions initializes the solenoid on the geocache.
 *
 */
void leds_init(void)
{
	// Enable clock
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	
	// Configure all pins as follows:
  // - MUX[2:0] = 001 : Alternative 1 (GPIO)
  // - DSE = 0 : Low drive strength
  // - PFE = 0 : Passive input filter is disabled
  // - SRE = 0 : Fast slew rate is configured
  // - PE = 0 : Internal pullup or pulldown resistor is not enabled
	PORTB->PCR[PIN_SOLENOID] = 0b00100000000;

	//Set the pins as output
	PTC->PDDR |= MASK(PIN_SOLENOID);
}

/*!
 * \brief Triggers the solenoid
 *
 * This functions switches on an LED.
 *
 */
void solenoid_trigger(void)
{
		PTB->PSOR = MASK(PIN_SOLENOID);
		delay_us(500000);
		PTB->PCOR = MASK(PIN_SOLENOID);
}
