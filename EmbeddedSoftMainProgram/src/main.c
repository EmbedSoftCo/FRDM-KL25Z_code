#include <MKL25Z4.h>
#include <stdio.h>

#include "pit.h"
#include "logging.h"
#include "tpm1.h"
#include "EEPROM.h"
#include "uart0.h"
#include "bme280.h"
#include "configScreen.h"
#include "gps.h"
#include "solenoid.h"
#include "logging.h"

//State variables
static bool varState0 = true;		//SETUP SCREEN -> Show with mode it needs to go in. (Admin or User)
static bool varState1 = false;	//USER state -> WAIT FOR FIX SCREEN and wait for pressing start
static bool varState2 = false;	//SHOW DISTANCE SCREEN -> update screen every 10 seconds
static bool varState3 = false;	//SHOW QUESTION SCREEN -> show question and let user choose answer
static bool varState4 = false;	//SHOW VICTORY SCREEN -> open box once and wait for confirm button to go to start screen
static bool varState5 = false;	//SHOW ADMIN SCREEN -> UART0 

//Prototypes
void state0(void);
void state1(void);
void state2(void);
void state3(void);
void state4(void);
void state5(void);

//Local variabels
static dataGps_t data;
static gameLocation_t location[4];
static dateTime_t timestamp;

static int32_t temp = 0x0000;
static char sTemp[8];
static int32_t hum = 0x0000;
static char sHum[8];
static double distance = 99;
static char sDistance[32];
static char sTime[32];

static uint8_t uartInput[512];
static uint16_t i = 0;

static uint8_t amountLocations;
static uint8_t counter;
static uint8_t punishTime;

																																		
/*!
 * \brief Main application
 */
int main(void)
{
		
		delay_us(1000000UL); //Start up
		
		//Init microcontroller settings
		uart0_init();
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
		*location[1].question = "Welkom,\nheb je er zin in?";
		*location[1].answer[1] = "Ja";
		*location[1].answer[2] = "Nee";
		*location[1].answer[3] = "Misschien";
		location[1].goodAnswer = 1;
		
		//location 2
		location[2].location.lat = 5950352;
		location[2].location.lon = 51988188;
		*location[2].question = "Bij welk gebouw\nstaan we?";
		*location[2].answer[1] = "29";
		*location[2].answer[2] = "31";
		*location[2].answer[3] = "26";
		location[2].goodAnswer = 3;
		
		//location 3
		location[3].location.lat = 5947739; 
		location[3].location.lon = 51988807;
		*location[3].question = "Hoeveel paaltjes\nstaan er?";
		*location[3].answer[1] = "3";
		*location[3].answer[2] = "6";
		*location[3].answer[3] = "9";
		location[3].goodAnswer = 2;
		
		amountLocations = 3;
		counter = 1;
		punishTime = 60;
		
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
			displayShowText("User mode", "");
			varState1 = true;
			varState0 = false;
		}
		else
		{
			displayShowText("Admin mode", "");
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
			displayShowText("WAITING FOR SATELLITE", "Don't hide ;)");
		}
		
		if(gpsFlag || data.state == 0)
		{
			gpsFlag = false;
			data = gps_newData();
		}
		
		if(data.state == FIX || data.state == GUESSING)
		{
			displayShowText("PRESS TO START!" , "Press center button.");
			if(sw_pressed(KEY_CENTER))
			{
				varState2 = true;
				varState1 = false;
				displayShowText("Ready?!", "");
				delay_us(500000);
				displayShowText("Set!", "");
				delay_us(500000);
				displayShowText("GO!", "");
				delay_us(250000);
			}
		}
	}
}
	

void state2(void) //SHOW DISTANCE SCREEN -> update screen when displayFlag is set
{
	bool runOnce = true;
	while(varState2 == true)
	{
		if(runOnce)
		{
			runOnce = false;
			displayFlag = true;
			logFlag = true;
			distance = 99;
		}
		if(gpsFlag)
		{
			gpsFlag = false;
			data = gps_newData();
			distance = gps_calculateDistance(location[counter].location, data.loc);
			sprintf(sDistance, "%.2lf", distance);
			delay_us(20);
			timestamp = convert_unix_timestamp(data.utc);
			if(timestamp.hour < 5 && timestamp.year < 2024)
			{
				// do nothing
			}
			else
			{
				sprintf(sTime, "%d:%d:%d", timestamp.hour, timestamp.minute, timestamp.second);
			}
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
			displayShowText("LOCATION FOUND!", "");
			delay_us(2000000);
		}
		if(logFlag ==	true)
		{
			// Reset the log flag
			logFlag = false;
			periodicLogging();
		}
	}
}

void state3(void) //SHOW QUESTION SCREEN -> show question and let user choose answer
{
	bool timerFinished = false;
	while(varState3 == true)
	{
		bool answer = false;
		answer = displayPuzzle(*location[counter].question,*location[counter].answer[1],*location[counter].answer[2],*location[counter].answer[3],&location[counter].goodAnswer);
		if(answer == true)
		{
			if(counter >= amountLocations)
			{
				displayShowText("GOOD ANSWER", "You have done it!");
				delay_us(2000000);
				varState4 = true;
				varState3 = false;
				return;
			}
			else
			{
				displayShowText("GOOD ANSWER", "Go to next location");
				delay_us(2000000);
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
				displayShowText("WRONG ANSWER", "Go to\nprevious location");
				counter--;
				timerFinished = true;
			}
			else 
			{
				uint16_t sec = 0;
				while(sec != punishTime)
				{
					if (gpsFlag)
					{
						gpsFlag = false;
						char tmp[20];
						sprintf(tmp, "Wait %u seconds", (punishTime-sec));
						displayShowText("WRONG ANSWER", tmp);
						sec++;
					}
				}
			}
			if(timerFinished)
			{
				varState2 = true;
				varState3 = false;
				return;
			}
		}
	}
}

void state4(void) //SHOW VICTORY SCREEN -> open box once and wait for confirm button to go to start screen 	-> TODO: BOX OPENING
{
	bool runOnce = true;
	while(varState4 == true)
	{
		if(runOnce == true)
		{
			runOnce = false;
			displayShowText("YOU WON", "press center button\nto continue");
			solenoid_trigger();
			periodicLogging();
			resetLog();
		}
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
	bool getState = false;
	while(varState5 == true)
	{
		if(runOnce == true)
		{
			runOnce = false;
			displayShowText("ADMIN MODE", "");
		}
		if(sw_pressed(KEY_DOWN))
		{
			solenoid_trigger();
		}
		if(uart0_num_rx_chars_available() > 0)
		{
			uartInput[0] = uart0_get_char();
			if(uartInput[0] == 's')
			{
				getState = true;
			}
			else if(uartInput[0] == 'g')
			{
				sendlogToUART();
			}
		}
		while(getState)
		{
			if(uart0_num_rx_chars_available() > 0)
			{
				if(i < 512)
				{
					uartInput[i] = uart0_get_char();
					i++;
				}
			}
			if(i >= sizeof(uartInput))
			{
				EEPROM_write_page(0,0,0, uartInput,sizeof(uartInput));
				memset(uartInput,0,sizeof(uartInput));
				i = 0;
				getState = false;
			}
		}
	}
}
