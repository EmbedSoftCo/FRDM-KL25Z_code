#include <MKL25Z4.h>
#include <stdio.h>

#include "pit.h"
#include "rg.h"
#include "tpm1.h"
#include "EEPROM.h"

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
    rg_init();
    pit_init();
    tpm1_init();
    
		EEPROM_init();
    
		
		uint8_t block = 0x0; 									// Block 0 = 0x0 	// Block 15 = 0xF
		uint8_t sector = 0x0;									// Sector 0 = 0x0 // Sector 15 = 0xF
		uint8_t page = 0x0; 									// Page 0 = 0x0 	// Page 7 = E00	
		
		
		EEPROM_write_page(block, sector, page, data_write, sizeof(data_write));
		
		EEPROM_read_page(block, sector, page, data_read, sizeof(data_read));
		
		bool green = false;
		if(data_read[4] == data_write[4])
		{
			green = true;
		}
	
    while(1)
    {
			rg_onoff(toggleLED,green);
    }
}
