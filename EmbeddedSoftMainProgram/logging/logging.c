#include "logging.h"
#include "EEPROM.h"
#include "bme280.h"
#include "gps.h"
#include <MKL25Z4.h>
#include <stdint.h>

#define RECORDS_OFFSET (1)

extern volatile bool logFlag;
static volatile struct LogData PageBuffer[32] = {0};
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
  // Reset  the log flag
  logFlag = false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/***************************************************
 * @brief log the data in the buffer to the EEPROM *
 ***************************************************/
void logInfo(void) {

  /* ================ Read data ================ */
	
	// numOfRecords that will be written into EEPROM
  struct MetaData md = {.countOfRecords = ((page) * 32) + ((sector)*(256)) + ((block-1)*(3840)) + numOfRecords-RECORDS_OFFSET};
		
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
	EEPROM_read_page(rBlock, rSector, rPage, (uint8_t*)PageBuffer, sizeof(readData));
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
 *@brief Read the EEPROM and send the data over de UART0 Bus in JSON format *
 ****************************************************************************/
void sendlogToUART(void)
{
	bool finalPage = false;
	
	uint8_t blocks = 0;
	uint8_t sectors = 0;
	uint8_t pages = 0;
	uint16_t pagesWritten = 0;
	uint16_t cnt = 0;
	
	//Read numOfRecords to read how much there is to read
	readPage(1,0,0);
	pagesWritten = (uint16_t)(PageBuffer[0].lattitude/16); // divided by 16 to go from records to pages. because there are 16 records in one page
	
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
			UART0->D = ((uint8_t*)PageBuffer)[iter];
		}		
		
		//Check if 
		if(cnt == pagesWritten) 
		{	
			finalPage = true;
			break;
		}
		cnt++;			
	}
	
	// Send last ASCII character (DEL)
	while(!(UART0->S1 & UART_S1_TDRE_MASK)){}
	UART0->D = 0x7f;
	// Send last ASCII character (DEL)
	while(!(UART0->S1 & UART_S1_TDRE_MASK)){}
	UART0->D = 0x7f;
	// Send last ASCII character (DEL)
	while(!(UART0->S1 & UART_S1_TDRE_MASK)){}
	UART0->D = 0x7f;
}
