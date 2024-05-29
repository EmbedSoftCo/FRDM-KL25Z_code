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

//State variables
static bool varState0 = true;		//SETUP SCREEN -> Show with mode it needs to go in. (Admin or User)
static bool varState1 = false;	//USER state -> WAIT FOR FIX SCREEN and wait for pressing start
static bool varState2 = false;	//SHOW DISTANCE SCREEN -> update screen every 10 seconds
static bool varState3 = false;	//SHOW QUESTION SCREEN -> show question and let user choose answer
static bool varState4 = false;	//SHOW VICTORY SCREEN -> open box once and wait for confirm button to go to start screen
static bool varState5 = false;	//SHOW GAME OVER SCREEN	-> Back to start screen
static bool varState6 = false;	//SHOW ADMIN SCREEN -> UART0 

void state0(void);
void state1(void);
void state2(void);
void state3(void);
void state4(void);
void state5(void);
void state6(void);

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
						
    while(1)
    {
			state0();
			state1();
			state2();
			state3();
			state4();
			state5();
			state6();
    }
}

void state0(void)
{
	//SETUP SCREEN -> Show with mode it needs to go in. (Admin or User)
	while(varState0 == true)
	{
		if(sw_pressed(KEY_LEFT))
		{
			uart0_send_string("0\n");
			varState1 = true;
			varState0 = false;
		}
	}
}

void state1(void)
{
	//USER state -> WAIT FOR FIX SCREEN and wait for pressing start
	while(varState1 == true)
	{
		if(sw_pressed(KEY_LEFT))
		{
			uart0_send_string("1\n");
			varState2 = true;
			varState1 = false;
		}
	}
}
	
void state2(void)
{
	//SHOW DISTANCE SCREEN -> update screen every 10 seconds
	while(varState2 == true)
	{
		if(sw_pressed(KEY_LEFT))
		{
			uart0_send_string("2\n");
			varState3 = true;
			varState2 = false;
		}
	}
}

void state3(void)
{
	//SHOW QUESTION SCREEN -> show question and let user choose answer
	while(varState3 == true)
	{
		if(sw_pressed(KEY_LEFT))
		{
			uart0_send_string("3\n");
			varState4 = true;
			varState3 = false;
		}
	}
}

void state4(void)
{
	//SHOW VICTORY SCREEN -> open box once and wait for confirm button to go to start screen
	while(varState4 == true)
	{
		if(sw_pressed(KEY_LEFT))
		{
			uart0_send_string("4\n");
			varState5 = true;
			varState4 = false;
		}
	}
}

void state5(void)
{
	//SHOW GAME OVER SCREEN	-> Back to start screen
	while(varState5 == true)
	{
		if(sw_pressed(KEY_LEFT))
		{
			uart0_send_string("5\n");
			varState6 = true;
			varState5 = false;
		}
	}
}

void state6(void)
{
	//SHOW ADMIN SCREEN -> UART0 
	while(varState6 == true)
	{
		if(sw_pressed(KEY_LEFT))
		{
			uart0_send_string("6\n");
			varState0 = true;
			varState6 = false;
		}
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
    logTemp8[0] = (uint8_t)(logTemp32 >> 24);
    logTemp8[1] = (uint8_t)(logTemp32 >> 16);
    logTemp8[2] = (uint8_t)(logTemp32 >> 8);
    logTemp8[3] = (uint8_t)(logTemp32 >> 0);
	
	  logHum8[0] = (uint8_t)(logHum32 >> 24);
    logHum8[1] = (uint8_t)(logHum32 >> 16);
    logHum8[2] = (uint8_t)(logHum32 >> 8);
    logHum8[3] = (uint8_t)(logHum32 >> 0);
	
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







// BEFORE WHILE LOOP CONCEPT
//		//Variables
//		uint8_t block = 0x0; 									// Block 0 = 0x0 	// Block 15 = 0xF
//		uint8_t sector = 0x0;									// Sector 0 = 0x0 // Sector 15 = 0xF
//		uint8_t page = 0x0; 									// Page 0 = 0x0 	// Page 7 = E00	
//		int32_t temp = 0x0000;
//		char sTemp[32];
//		int32_t hum = 0x0000;
//		char sHum[32];
//		bool state = false;

// WHILE MAIN LOOP CONCEPT
//			if(sw_pressed(KEY_LEFT) == 0 && state == true)
//			{
//				state = false;
//				temp = get_temperature();
//				sprintf(sTemp, "%d.%d\r\n", (temp/100),(temp - ((temp/100)*100)));
//				uart0_send_string(sTemp);
//				hum = get_humidity();
//				sprintf(sHum, "%d.%d\r\n", (hum/100),(hum - ((hum/100)*100)));
//				uart0_send_string(sTemp);
//				delay_us(5);
//				displayDistance("?", "london", sTemp, sHum);
//			}
//			else if (sw_pressed(KEY_LEFT) == 1 && sw_pressed(KEY_RIGHT) == 1)
//			{
//				state = true;
//			}