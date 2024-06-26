/*! ***************************************************************************
 *
 * \brief     Low level driver for the switches
 * \file      switches.c
 * \author    Hugo Arends
 * \date      June 2021
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
#include "switches.h"
#include "delay.h"

volatile bool sw_clicked_flag_1 = false;
volatile bool sw_clicked_flag_2 = false;
volatile bool sw_clicked_flag_3 = false;
volatile bool sw_clicked_flag_4 = false;

/*!
 * \brief Initialises the switches on the shield
 *
 * This functions initializes the switches on the shield.
 */
void sw_init(void)
{
	// Enables clocks to PORTs
	//SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
	// Configure the pin as follows:
	// - IRQC[3:0] = 1010 : Interrupt on falling edge
	// - MUX[2:0] = 001 : Alternative 1 (GPIO)
	// - DSE = 0 : Low drive strength
  // - PFE = 0 : Passive input filter is disabled
  // - SRE = 0 : Fast slew rate is configured
  // - PE = 1 : Internal pullup or pulldown resistor enabled
  // - PS = 1 : Internal pullup
	PORTC->PCR[3] = PORT_PCR_IRQC(10) | PORT_PCR_MUX(1) | PORT_PCR_PS(1) | PORT_PCR_PE(1);
	PORTC->PCR[6] = PORT_PCR_IRQC(10) | PORT_PCR_MUX(1) | PORT_PCR_PS(1) | PORT_PCR_PE(1);
	PORTC->PCR[4] = PORT_PCR_IRQC(10) | PORT_PCR_MUX(1) | PORT_PCR_PS(1) | PORT_PCR_PE(1);
	PORTC->PCR[0] = PORT_PCR_IRQC(10) | PORT_PCR_MUX(1) | PORT_PCR_PS(1) | PORT_PCR_PE(1);
	// Set port pint to input
	PTC->PDDR &= ~(1<<3);
	PTC->PDDR &= ~(1<<6);
	PTC->PDDR &= ~(1<<4);
	PTC->PDDR &= ~(1<<0);

}

/*!
 * \brief Check if a switch is pressed
 *
 * This functions checks if a switch is pressed. The function simply checks the
 * value of the switch at the moment the function is called. It doesn't
 * remember if the switch has been pressed.
 *
 * \param[in]  key  Key that will be checked, must be of type ::key_t
 *
 * \return True if the key is pressed, false otherwise
 */
bool sw_pressed(const key_t key)
{
	bool ret = false;
	
	switch(key)
	{
		case KEY_LEFT : { ret = ((PTC->PDIR & (1<<KEY_LEFT)) == 0); 		delay_us(30000);} break;
		case KEY_DOWN : { ret = ((PTC->PDIR & (1<<KEY_DOWN)) == 0); 		delay_us(30000);} break;
		case KEY_RIGHT : {ret = ((PTC->PDIR & (1<<KEY_RIGHT)) == 0); 		delay_us(30000);} break;
		case KEY_UP : { ret = ((PTC->PDIR & (1<<KEY_UP)) == 0); 				delay_us(30000);} break;
		case KEY_CENTER : { ret = ((PTC->PDIR & (1<<KEY_CENTER)) == 0); delay_us(30000);} break;
		
	}
    return ret;
}

