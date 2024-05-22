#include <MKL25Z4.h>
#include <stdio.h>

#include "pit.h"
#include "rg.h"
#include "tpm1.h"
#include "EEPROM.h"
#include "uart0.h"
#include "bme280.h"

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
	
		uart0_send_string("code Initialised!\r\n");
		
		uint8_t block = 0x0; 									// Block 0 = 0x0 	// Block 15 = 0xF
		uint8_t sector = 0x0;									// Sector 0 = 0x0 // Sector 15 = 0xF
		uint8_t page = 0x0; 									// Page 0 = 0x0 	// Page 7 = E00	
		int32_t temp = 0x0000;
		char antwoord[32];
		
		EEPROM_write_page(block, sector, page, data_write, sizeof(data_write));
	
		EEPROM_read_page(block, sector, page, data_read, sizeof(data_read));
		
		bool green = false;
		if(data_read[4] == data_write[4])
		{
			green = true;
		}
		
		delay_us(100000UL);
		
    while(1)
    {
			temp = get_temperature();
			
			sprintf(antwoord, "%d\r\n", temp);
			
			uart0_send_string(antwoord);
			rg_onoff(toggleLED,green);
			delay_us(100000UL);
    }
}