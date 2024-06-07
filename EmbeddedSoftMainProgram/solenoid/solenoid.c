/*! ***************************************************************************
 *
 * \brief     Low level driver for the LEDs
 * \file      solenoid.c
 * \author    Thijs Hulshof
 * \date      Juni 2024
 *
 *****************************************************************************/
#include "solenoid.h"

/*!
 * \brief Initialises the solenoid
 *
 * This functions initializes the solenoid on the geocache.
 *
 */
void solenoid_init(void)
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
	PTB->PDDR |= MASK(PIN_SOLENOID);
	
	//Set pin low
	PTB->PCOR = MASK(PIN_SOLENOID);
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
