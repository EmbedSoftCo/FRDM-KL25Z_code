#include <MKL25Z4.h>
#include <stdio.h>

#include "pit.h"
#include "rg.h"
#include "tpm1.h"
#include "EEPROM.h"
#include "uart0.h"
#include "bme280.h"
#include "configScreen.h"
#include "gps.h"

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
		int32_t hum = 0x0000;
		char sHum[32];
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
		
		uart0_send_string("Init GPS\n");
		gps_init();
		
		uart0_send_string("code Initialised!\r\n");
		
		EEPROM_write_page(block, sector, page, data_write, sizeof(data_write));
	
		EEPROM_read_page(block, sector, page, data_read, sizeof(data_read));
		
		bool green = false;
		if(data_read[4] == data_write[4])
		{
			green = true;
		}
		
		gps_newData();
				
    while(1)
    {
			if(sw_pressed(KEY_LEFT) == 0 && state == true)
			{
				state = false;
				temp = get_temperature();
				sprintf(sTemp, "%d.%d\r\n", (temp/100),(temp - ((temp/100)*100)));
				uart0_send_string(sTemp);
				hum = get_humidity();
				sprintf(sHum, "%d.%d\r\n", (hum/100),(hum - ((hum/100)*100)));
				uart0_send_string(sTemp);
				delay_us(5);
				displayDistance("?", "london", sTemp, sHum);
			}
			else if (sw_pressed(KEY_LEFT) == 1 && sw_pressed(KEY_RIGHT) == 1)
			{
				state = true;
			}
		
			rg_onoff(toggleLED, green);
    }
}

void logInfo(void)
{
	uint32_t logTemp32 = 0x00000;
	uint32_t logHum32 = 0x00000;
	
	uint8_t logTemp8[4];
	uint8_t logHum8[4];
	
	uint8_t block = 0x0; 		// Block 0 = 0x0 	// Block 15 = 0xF
	uint8_t sector = 0x0;		// Sector 0 = 0x0 // Sector 15 = 0xF
	uint8_t page = 0x0; 		// Page 0 = 0x0 	// Page 7 = E00	
	
/* ================ Read data ================ */	
	logTemp32 = get_temperature();	// Get temperature reading
	logHum32 = get_humidity();			// Get humidity reading
	
	// Split uint32_t temp & hum to 4 bytes
    logTemp8[0] = (logTemp32 >> 24);
    logTemp8[1] = (logTemp32 >> 16);
    logTemp8[2] = (logTemp32 >> 8);
    logTemp8[3] = (logTemp32 >> 0);
	
	  logHum8[0] = (logHum32 >> 24);
    logHum8[1] = (logHum32 >> 16);
    logHum8[2] = (logHum32 >> 8);
    logHum8[3] = (logHum32 >> 0);
	
	//ToDo GPS data
	
	//ToDo prepare data for EEPROM
	//data_write[] = 
	
/* ================ Write data to EEPROM ================ */		
	/*
	 * usage configuration of EEPROM
	 * 
	 * block 0
	 * sector 0
	 * page 0 - 7
	 *		page 0    = configuration data
	 *		page 1 - 7 = gamedata
	*/
	// Page 0 is config
	EEPROM_write_page(block, sector, page, data_write, sizeof(data_write));
	
	// Reset  the log flag
	logFlag = false;
}