#include <MKL25Z4.h>
#include <stdio.h>

#include "pit.h"
#include "logging.h"
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
//static uint8_t data_read[511] = {0};

/*!
 * \brief Main application
 */
int main(void)
{
		
		delay_us(1000000UL); //Start up
		
		//Init microcontroller settings
		uart0_init();
		rg_init();
		pit_init();
		tpm1_init();  
		
		//Init hardware components
		EEPROM_init();
		bme280_init();
		displayInit();
		gps_init();
	
		//Wait for starting to show welcome screen
		delay_us(2000000);
						
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

void state0(void) //SETUP SCREEN -> Show with mode it needs to go in. (Admin or User)
{
	while(varState0 == true)
	{
		bool answer;
		answer = displayStart();
		
		if(answer)
		{
			displayShowText("user");
			varState1 = true;
			varState0 = false;
		}
		else
		{
			displayShowText("admin");
			varState6 = true;
			varState0 = false;
		}
	}
}


void state1(void) //USER state -> WAIT FOR FIX SCREEN and wait for pressing start
{
	bool runOnce = true;
	dataGps_t data;
	while(varState1 == true)
	{
		if(runOnce == true)
		{
			runOnce = false;
			displayShowText("Waiting for Satelites");
		}
		
		if(gpsFlag)
		{
			gpsFlag = false;
			gps_newData();
			data = gps_getData();
			char sPrint[32];
			sprintf(sPrint, "%d", data.state);
			uart0_send_string(sPrint);
		}
		
		if(data.state == FIX || data.state == GUESSING)
		{
			displayShowText("Press center button to start the game!");
		}
		
		if(sw_pressed(KEY_CENTER) && (data.state == FIX|| data.state == GUESSING))
		{
			varState2 = true;
			varState1 = false;
			displayShowText("Ready?!");
			delay_us(500000);
			displayShowText("Set!");
			delay_us(500000);
			displayShowText("GO!");
			delay_us(250000);
		}
	}
}
	

void state2(void) //SHOW DISTANCE SCREEN -> update screen when displayFlag is set 													-> TODO: CHANGE CENTER BUTTON FOR CORRECT LOCATION + ADD MORE LOCATIONS
{
	int32_t temp = 0x0000;
	char sTemp[32];
	int32_t hum = 0x0000;
	char sHum[32];
	point_t smulhoek;
	smulhoek.lat = 5198774;
	smulhoek.lon = 594229;
	dataGps_t data;
	char sDistance[32];
	while(varState2 == true)
	{
		if(displayFlag)
		{
				gps_newData();
				data = gps_getData();
				displayFlag = false;
				temp = get_temperature();
				sprintf(sTemp, "%d.%d\r\n", (temp/100),(temp - ((temp/100)*100)));
				hum = get_humidity();
				sprintf(sHum, "%d.%d\r\n", (hum/100),(hum - ((hum/100)*100)));
				sprintf(sDistance, "%d", gps_calculateDistance(smulhoek, data.loc));
				delay_us(5);
				displayDistance(sDistance, "london", sTemp, sHum);
		}
		
		if(sw_pressed(KEY_CENTER))
		{
			varState3 = true;
			varState2 = false;
			delay_us(5000);
		}
	}
}


void state3(void) //SHOW QUESTION SCREEN -> show question and let user choose answer 												-> TODO: CHANGE PUZZLE QUESTION AND ANSWERS + ADD MORE LOCATIONS
{
	while(varState3 == true)
	{
		int goodAnswer = 2;
		bool answer = false;
		answer = displayPuzzle("Smulhoek?","Nee","Ja","Misschien",&goodAnswer);
		if(answer == true)
		{
			varState4 = true;
			varState3 = false;
		}
	}
}


void state4(void) //SHOW VICTORY SCREEN -> open box once and wait for confirm button to go to start screen 	-> TODO: ALL
{
	
	while(varState4 == true)
	{
		if(gpsFlag)
		{
			gpsFlag = false;
			displayShowText("4");
			varState5 = true;
			varState4 = false;
		}
	}
}


void state5(void)	//SHOW GAME OVER SCREEN	-> Back to start screen 																					-> TODO: ALL
{
	while(varState5 == true)
	{
		if(gpsFlag)
		{
			gpsFlag = false;
			displayShowText("5");
			varState0 = true;
			varState5 = false;
		}
	}
}


void state6(void) //SHOW ADMIN SCREEN -> UART0 																															-> TODO: ADD UART FUNCTIONS
{
	bool runOnce = true;
	while(varState6 == true)
	{
		if(runOnce == true)
		{
			displayShowText("ADMIN MODE");
		}
		if(uart0_num_rx_chars_available() > 0)
		{
			uart0_get_char();
		}
		rg_onoff(true,false);
	}
}

