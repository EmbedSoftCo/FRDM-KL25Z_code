#include <MKL25Z4.h>
#include <stdio.h>

#include "pit.h"
#include "rg.h"
#include "tpm1.h"
#include "uart0.h"
#include "configScreen.h"
#include "gps.h"
#include "EEPROM.h"
#include "bme280.h"
#include "logging.h"

uint8_t finishedRoute;

/*!
 * \brief Main application
 */
int main(void)
{
		//Variables
		int32_t temp = 0x0000;
		int32_t hum = 0x0000;	
	
		char sTemp[32];
		char sHum[32];
		bool state = false;
		bool green = false;
	
		delay_us(1000000UL); //Start up
		uart0_init();
		
		uart0_send_string("Init RG leds\n");
		rg_init();
		
    uart0_send_string("Init PIT\n");
		pit_init();
    
		uart0_send_string("Init TPM1\n");
		tpm1_init();  
		
		uart0_send_string("Init EEPROM\n");
		EEPROM_init();
    
		uart0_send_string("Init BME 280\n");
		bme280_init();
		
		uart0_send_string("Init Display\n");
		displayStart();
		
		uart0_send_string("Init GPS\n");
		gps_init();
		
		uart0_send_string("code Initialised!\r\n");
				
		//gps_newData();
				
    while(1)
    {
		//	if(logFlag)
			//{ 
				PerodicLogging();
				
				green = !green; // toggle green led
				
				rg_onoff(toggleLED, green);
		//	}
			

    }
}