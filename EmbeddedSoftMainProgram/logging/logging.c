#include "logging.h"
#include "EEPROM.h"
#include "bme280.h"
#include "gps.h"
#include <MKL25Z4.h>
#include <stdint.h>

#define RECORDS_OFFSET (1)

static struct LogData PageBuffer[32] = {0};
static struct LogData ReadPage[32] = {0};
static uint16_t numOfRecords = 0;

static uint8_t block = 0x1;  // Block 0 = 0x0 	// Block 15 = 0xF
static uint8_t sector = 0x0; // Sector 0 = 0x0 // Sector 15 = 0xF
static uint8_t page = 0x0; // Page 0 = 0x0 	// Page 7 = E00

static uint8_t readData[512];

void logInfo(void);
static void readPage(uint8_t rBlock, uint8_t rSector, uint8_t rPage);
static void convert_page_to_String(uint8_t* data);

/********************************************
 * @brief call regularly to fill the buffer *
 ********************************************/
void periodicLogging(void) {

  uint32_t logTemp32 = get_temperature(); // Get temperature reading
  uint32_t logHum32 = get_humidity();     // Get humidity reading
  dataGps_t gpsData = gps_getData();			// Get GPS location

	// Fill tmpbuf with GPS location, temperature and humidity
  struct LogData tmpbuf = {.lattitude = gpsData.loc.lat,  
                           .longtitude = gpsData.loc.lon, 
                           .temperature = logTemp32,
                           .humidity = logHum32};
	
// Fill PageBuffer array with new data											 
  PageBuffer[numOfRecords + RECORDS_OFFSET] = tmpbuf;
	
	for(uint16_t iter=0; iter<=sizeof(tmpbuf)*32;iter++)
	{
		while(!(UART0->S1 & UART_S1_TDRE_MASK)){}
		UART0->D = ((uint8_t*)PageBuffer)[iter];
	}									 
	
  numOfRecords++;
	
	//Log the data to EEPROM
	logInfo();
													 
  //Check if numOfRecords is 32													 
  if (numOfRecords == 32) {
    numOfRecords = 0;
		if(page == 7)
		{
			page = 0;
			if(sector == 7)
			{
				if(block < 15)
				{
					block++;
				}				
			}
			else
			{
				sector++;
			}
		}
		else
		{
			page++;
		}
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/***************************************************
 * @brief log the data in the buffer to the EEPROM *
 ***************************************************/
void logInfo(void) {

  /* ================ Read data ================ */
	
	// numOfRecords that will be written into EEPROM
  struct MetaData md = {.countOfRecords = numOfRecords-RECORDS_OFFSET};
		
	// Convert Metadata struct to a logdata struct and save into bufferpage 0
  PageBuffer[0] = *(struct LogData *)&md;

  /* ================ Write data to EEPROM ================ */
	
	EEPROM_write_page(block, sector, page, (uint8_t*)PageBuffer, sizeof(PageBuffer));
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**************************************************************************
 * @brief Read the page in EEPROM, save the read data to readData[] array *
 **************************************************************************/
void readPage(uint8_t rBlock, uint8_t rSector, uint8_t rPage)
{
	EEPROM_read_page(rBlock, rSector, rPage, (uint8_t*)ReadPage, sizeof(readData));
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
 *@brief Read the EEPROM and send the data over de UART0 Bus in JSON format *
 ****************************************************************************/
void sendlogToUART(void)
{
	bool finalPage = false;
	
	uint8_t blocks = 1;
	uint8_t sectors = 0;
	uint8_t pages = 0;
	
	//Read numOfRecords to read how much there is to read	
	while(!finalPage)	
	{
		readPage(blocks, sectors, pages);
		
		if(pages == 7)
		{
			pages = 0;
			if(sectors == 7)
			{
				blocks++;			
			}
			else
			{
				sectors++;
			}
		}
		else
		{
			pages++;
		}
		
		// Send EEPROM Page
		for(uint16_t iter=0; iter<=512;iter++)
		{
			while(!(UART0->S1 & UART_S1_TDRE_MASK)){}
			UART0->D = ((uint8_t*)ReadPage)[iter];
		}		
		
		// Send last ASCII character (DEL)
		while(!(UART0->S1 & UART_S1_TDRE_MASK)){}
		UART0->D = 0x7f;
		
		//Check if 
		if(ReadPage[0].lattitude < 31) 
		{	
			finalPage = true;
		}			
	}
	
	// Send last ASCII character (DEL)
	while(!(UART0->S1 & UART_S1_TDRE_MASK)){}
	UART0->D = 0x7f;
	// Send last ASCII character (DEL)
	while(!(UART0->S1 & UART_S1_TDRE_MASK)){}
	UART0->D = 0x7f;
}

/****************************************************************************
 *@brief Reset logging for the next game.
 ****************************************************************************/
void resetLog(void)
{
	numOfRecords = 0;
	block = 0x1;  
	sector = 0x0; 
	page = 0x0;
}
