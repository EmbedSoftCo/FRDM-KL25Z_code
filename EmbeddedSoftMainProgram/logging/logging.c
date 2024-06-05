#include "logging.h"
#include "EEPROM.h"
#include "bme280.h"
#include "gps.h"
#include <MKL25Z4.h>
#include <stdint.h>

#define RECORDS_OFFSET (1)

extern volatile bool logFlag;
static volatile struct LogData PageBuffer[32] = {0};
static uint32_t numOfRecords = 0;

extern uint8_t finishedRoute;
static uint8_t page = 0x0; // Page 0 = 0x0 	// Page 7 = E00

static uint8_t readData[512];
static char preparedData[512];

static void logInfo(void);
static void readPage(uint8_t page);
static void convert_page_to_String(uint8_t* data);

/********************************************
 * @brief call regularly to fill the buffer *
 ********************************************/
void PerodicLogging(void) {

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

  /* Check if numOfRecords is 32, or
		 Game is finished. Log then the data to EEPROM
   */													 
  if (numOfRecords == 32 || finishedRoute == 1) {
    logInfo();
    numOfRecords = 0;
    page++;
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/***************************************************
 * @brief log the data in the buffer to the EEPROM *
 ***************************************************/
void logInfo(void) {
  uint8_t block = 0x0;  // Block 0 = 0x0 	// Block 15 = 0xF
  uint8_t sector = 0x0; // Sector 0 = 0x0 // Sector 15 = 0xF

  /* ================ Read data ================ */
	
	// numOfRecords that will be written into EEPROM
  struct MetaData md = {.countOfRecords = numOfRecords-RECORDS_OFFSET};

	// Convert Metadata struct to a logdata struct and save into bufferpage 0
  PageBuffer[0] = *(struct LogData *)&md;

  /* ================ Write data to EEPROM ================ */
	
   EEPROM_write_page(block, sector, page, (uint8_t*) PageBuffer, sizeof(PageBuffer));

  // Reset  the log flag
  logFlag = false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**************************************************************************
 * @brief Read the page in EEPROM, save the read data to readData[] array *
 **************************************************************************/
void readPage(uint8_t page)
{
	uint8_t block = 0x0;
  uint8_t sector = 0x0;
	
	EEPROM_read_page(block, sector, page, readData, sizeof(readData));
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*******************************************************
 * @brief Convert uint8_t array to an String,          *
 *				This will be used for sending data over UART *
 *******************************************************/
void convert_page_to_String(uint8_t* data)
{
	sprintf(preparedData, "%d\r\n", data);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
 *@brief Read the EEPROM and send the data over de UART0 Bus in JSON format *
 ****************************************************************************/
void sendlogToUART(void)
{
	bool finalPage = false;
	uint8_t pages = 0;
	uint32_t writtenPages = 0;
	
	// start JSON UART
	uart0_send_string("[");
	
	// Parse data over UART in JSON format
	while(!finalPage)
	{
		// Open JSON object
		uart0_send_string("{");
		
		// read and convert EEPROM data to String and send it over UART
		readPage(pages);
		convert_page_to_String(readData);
		uart0_send_string(preparedData);
		
		// Close JSON object
		uart0_send_string("}"); 
		
		// Get the metadata and save it in writtenPages 
		writtenPages = ((readData[0] << 24) | (readData[1] << 16) | (readData[2] << 8) | (readData[3] << 0));
		
		if(writtenPages == 0)
		{
			finalPage = true;
		}
		else
		{
			pages++; // Go to next page in EEPROM
			
			uart0_send_string(","); // Prepare next JSON object
		}
	}
	
	// END JSON UART
	uart0_send_string("]");
}
