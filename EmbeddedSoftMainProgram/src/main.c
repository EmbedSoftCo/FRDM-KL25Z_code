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
#include "solenoid.h"

//State variables
static bool varState0 = true;		//SETUP SCREEN -> Show with mode it needs to go in. (Admin or User)
static bool varState1 = false;	//USER state -> WAIT FOR FIX SCREEN and wait for pressing start
static bool varState2 = false;	//SHOW DISTANCE SCREEN -> update screen every 10 seconds
static bool varState3 = false;	//SHOW QUESTION SCREEN -> show question and let user choose answer
static bool varState4 = false;	//SHOW VICTORY SCREEN -> open box once and wait for confirm button to go to start screen
static bool varState5 = false;	//SHOW ADMIN SCREEN -> UART0 

void state0(void);
void state1(void);
void state2(void);
void state3(void);
void state4(void);
void state5(void);

static dataGps_t data;
static int32_t temp = 0x0000;
static char sTemp[10];
static int32_t hum = 0x0000;
static char sHum[10];
static char sDistance[32];
static char sTime[32];\
static dateTime_t timestamp;

static double distance = 99;
static gameLocation_t location[4];
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
		solenoid_init();
		
		//Init hardware components
		EEPROM_init();
		bme280_init();
		displayInit();
		gps_init();
		
		//Default settings of the game
	
		//location 1
		location[1].location.lat = 5948933; 
		location[1].location.lon = 51989406;
		*location[1].question = "Smulhoek?";
		*location[1].answer[1] = "Ja";
		*location[1].answer[2] = "Nee";
		*location[1].answer[3] = "Misschien";
		location[1].goodAnswer = 1;
		
		//location 2
		location[2].location.lat = 5950352;
		location[2].location.lon = 51988188;
		*location[2].question = "Testvraag 2?";
		*location[2].answer[1] = "Ja";
		*location[2].answer[2] = "Nee";
		*location[2].answer[3] = "Misschien";
		location[2].goodAnswer = 1;
		
		//location 3
		location[3].location.lat = 5947739; 
		location[3].location.lon = 51988807;
		*location[3].question = "Testvraag 3?";
		*location[3].answer[1] = "Ja";
		*location[3].answer[2] = "Nee";
		*location[3].answer[3] = "Misschien";
		location[3].goodAnswer = 1;
		
		amountLocations = 3;
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
			varState5 = true;
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
			}
		}
	}
}
	

void state2(void) //SHOW DISTANCE SCREEN -> update screen when displayFlag is set
{
	displayFlag = true;
	distance = 99;
	while(varState2 == true)
	{
		if(gpsFlag)
		{
			gpsFlag = false;
			data = gps_newData();
			distance = gps_calculateDistance(location[counter].location, data.loc);
			sprintf(sDistance, "%.2lf", distance);
			delay_us(20);
			timestamp = convert_unix_timestamp(data.utc);
			if(timestamp.hour < 5 || timestamp.minute < 0 || timestamp.second < 0 || timestamp.year > 2024)
			{
				// do nothing
			}
			else
			{
				sprintf(sTime, "%d:%d:%d", timestamp.hour, timestamp.minute, timestamp.second);
			}
			uart0_send_string(sDistance);
			uart0_put_char('\n');
			uart0_send_string(sTime);
			uart0_send_string("\n");
		}
		if(displayFlag)
		{
			displayFlag = false;
			temp = get_temperature();
			sprintf(sTemp, "%d.%d\r\n", (temp/100),(temp - ((temp/100)*100)));
			hum = get_humidity();
			sprintf(sHum, "%d.%d\r\n", (hum/100),(hum - ((hum/100)*100)));
			delay_us(5);
			displayDistance(sDistance, sTime, sTemp, sHum);
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
				return;
			}
			else
			{
				counter++;
				varState2 = true;
				varState3 = false;
				return;
			}
		}
		else
		{
			if(counter >= 2)
			{
				counter--;
			}
				varState2 = true;
				varState3 = false;
				return;
		}
	}
}

void state4(void) //SHOW VICTORY SCREEN -> open box once and wait for confirm button to go to start screen 	-> TODO: BOX OPENING
{
	displayShowText("YOU WON, press center button to continue");
	
	solenoid_trigger();
	
	while(varState4 == true)
	{
		if(sw_pressed(KEY_CENTER))
		{
			counter = 1;
			varState0 = true;
			varState4 = false;
		}
	}
}


void state5(void) //SHOW ADMIN SCREEN -> UART0 																															-> TODO: ADD UART FUNCTIONS
{
	bool runOnce = true;
	while(varState5 == true)
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

