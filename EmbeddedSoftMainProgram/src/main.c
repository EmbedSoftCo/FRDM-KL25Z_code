#include <MKL25Z4.h>
#include <stdio.h>

#include "pit.h"
#include "rg.h"
#include "tpm1.h"
#include "EEPROM.h"
#include "uart0.h"
#include "bme280.h"
#include "configScreen.h"

static uint8_t data_write[511] = {1,2,3,4,5,6,7,8,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,8,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,
																	1,2,3,4,5,6,7,8,
                                  };
static uint8_t data_read[511] = {0};


/*!
 * \brief Main application
 */
int main(void)
{
		//Variables
		uint8_t block = 0x0; 									// Block 0 = 0x0 	// Block 15 = 0xF
		uint8_t sector = 0x0;									// Sector 0 = 0x0 // Sector 15 = 0xF
		uint8_t page = 0x0; 									// Page 0 = 0x0 	// Page 7 = E00	
		int32_t temp = 0x0000;
		char sTemp[32];
		bool state = false;
	
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
		
		uart0_send_string("code Initialised!\r\n");
		
		EEPROM_write_page(block, sector, page, data_write, sizeof(data_write));
	
		EEPROM_read_page(block, sector, page, data_read, sizeof(data_read));
		
		bool green = false;
		if(data_read[4] == data_write[4])
		{
			green = true;
		}
				
    while(1)
    {
			if(sw_pressed(KEY_LEFT) == 0 && state == true)
			{
				state = false;
				temp = get_temperature();
				sprintf(sTemp, "%d.%d\r\n", (temp/100),(temp - ((temp/100)*100)));
				uart0_send_string(sTemp);
				delay_us(5);
				displayDistance("?", "london", sTemp);
			}
			else if (sw_pressed(KEY_LEFT) == 1 && sw_pressed(KEY_RIGHT) == 1)
			{
				state = true;
			}
		
			rg_onoff(toggleLED,green);
    }
}
