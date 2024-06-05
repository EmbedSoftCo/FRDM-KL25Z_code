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

volatile static dataGps_t data;
volatile static	double distance = 99;
//volatile static dateTime_t date;
volatile static gameLocation_t location[4];
static int amountLocations;
static int counter;

																																		
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
		
		//Default settings of the game
	
		//location 1
		location[1].location.lat = 6312330;
		location[1].location.lon = 52049643;
		*location[1].question = "Smulhoek?";
		*location[1].answer[1] = "Ja";
		*location[1].answer[2] = "Nee";
		*location[1].answer[3] = "Misschien";
		location[1].goodAnswer = 1;
		
		//location 2
		location[2].location.lat = 6312330;
		location[2].location.lon = 52049643;
		*location[2].question = "Testvraag 2?";
		*location[2].answer[1] = "Ja";
		*location[2].answer[2] = "Nee";
		*location[2].answer[3] = "Misschien";
		location[2].goodAnswer = 1;
		
		amountLocations = 1;
		counter = 1;
	
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
	while(varState1 == true)
	{
		if(runOnce == true)
		{
			runOnce = false;
			displayShowText("Waiting for Satelites");
		}
		
		if(gpsFlag || data.state == 0)
		{
			gpsFlag = false;
			data = gps_newData();
		}
		
		if(data.state == FIX || data.state == GUESSING)
		{
			displayShowText("Press center button to start the game!");
			while(!sw_pressed(KEY_CENTER))
			{
				if(sw_pressed(KEY_CENTER))
				{
					varState2 = true;
					varState1 = false;
					displayShowText("Ready?!");
					delay_us(500000);
					displayShowText("Set!");
					delay_us(500000);
					displayShowText("GO!");
					delay_us(250000);
					break;
				}
			}
		}
	}
}
	

void state2(void) //SHOW DISTANCE SCREEN -> update screen when displayFlag is set
{
	int32_t temp = 0x0000;
	char sTemp[10];
	int32_t hum = 0x0000;
	char sHum[10];
	char sDistance[64];
	displayFlag = true;
	while(varState2 == true)
	{
		if(gpsFlag)
		{
			gpsFlag = false;
			data = gps_newData();
			distance = gps_calculateDistance(location[counter].location, data.loc);
		}
		if(displayFlag)
		{
			displayFlag = false;
			temp = get_temperature();
			sprintf(sTemp, "%d.%d\r\n", (temp/100),(temp - ((temp/100)*100)));
			hum = get_humidity();
			sprintf(sHum, "%d.%d\r\n", (hum/100),(hum - ((hum/100)*100)));
			sprintf(sDistance, "%.2lf", distance);
			//date = convert_unix_timestamp(data.utc);
			delay_us(5);
			uart0_send_string(sDistance);
			uart0_send_string("\n");
			displayDistance(sDistance, "Tijd om te lopen", sTemp, sHum);
		}
		if(distance <= MAXRADIUS)
		{
			varState3 = true;
			varState2 = false;
			displayShowText("Location found!");
			delay_us(5000);
		}
	}
}

void state3(void) //SHOW QUESTION SCREEN -> show question and let user choose answer
{
	while(varState3 == true)
	{
		bool answer = false;
		answer = displayPuzzle(*location[counter].question,*location[counter].answer[1],*location[counter].answer[2],*location[counter].answer[3],&location[counter].goodAnswer);
		if(answer == true)
		{
			if(counter >= amountLocations)
			{
				varState4 = true;
				varState3 = false;
			}
			else
			{
				counter++;
			}
		}
		else
		{
			varState5 = true;
			varState3 = false;
		}
	}
}

void state4(void) //SHOW VICTORY SCREEN -> open box once and wait for confirm button to go to start screen 	-> TODO: BOX OPENING
{
	displayShowText("YOU WON, press center button to continu");
	while(varState4 == true)
	{
		if(sw_pressed(KEY_CENTER))
		{
			varState0 = true;
			varState4 = false;
		}
	}
}


void state5(void)	//SHOW GAME OVER SCREEN	-> Back to start screen
{
	displayShowText("GAME OVER, press center button to continu");
	while(varState5 == true)
	{
		if(sw_pressed(KEY_CENTER))
		{
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

