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

/**
 * @brief call regularly to fill the buffer
 */
void PerodicLogging(void) {

  uint32_t logTemp32 = get_temperature(); // Get temperature reading
  uint32_t logHum32 = get_humidity();     // Get humidity reading
  dataGps_t gpsData = gps_getData();			// Get GPS location

  struct LogData tmpbuf = {.lattitude = gpsData.loc.lat,  
                           .longtitude = gpsData.loc.lon, 
                           .temperature = logTemp32,
                           .humidity = logHum32};

  PageBuffer[numOfRecords + RECORDS_OFFSET] = tmpbuf;

  numOfRecords++;

  if (numOfRecords == 32 || finishedRoute == 1) {
    logInfo();
    numOfRecords = 0;
    page++;
  }
}

/***
 * @brief log the data in the buffer to the eeprom
 */
void logInfo(void) {
  uint8_t block = 0x0;  // Block 0 = 0x0 	// Block 15 = 0xF
  uint8_t sector = 0x0; // Sector 0 = 0x0 // Sector 15 = 0xF

  /* ================ Read data ================ */

  struct MetaData md = {.countOfRecords = numOfRecords-RECORDS_OFFSET};

  PageBuffer[0] = *(struct LogData *)&md; //* ( long * ) &

  /* ================ Write data to EEPROM ================ */
  /*
   * usage configuration of EEPROM
   *
   * block 0
   * sector 0
   * page 0 - 7
   *		page 0     = configuration data
   *		page 1 - 7 = gamedata
   */
  // Page 0 is config
	
   EEPROM_write_page(block, sector, page, (uint8_t*) PageBuffer, sizeof(PageBuffer));

  // Reset  the log flag
  logFlag = false;
}

void readPage(uint8_t page)
{
	uint8_t block = 0x0;  // Block 0 = 0x0 	// Block 15 = 0xF
  uint8_t sector = 0x0; // Sector 0 = 0x0 // Sector 15 = 0xF
	
	EEPROM_read_page(block, sector, page, readData, sizeof(readData));
}

void convert_page_to_String(uint8_t* data)
{
	sprintf(preparedData, "%d\r\n", data);
}

/*
 * Read the EEPROM and send the data over de UART0 Bus
*/
void sendlogToUART(void)
{
	bool finalPage = false;
	uint8_t pages = 0;
	uint32_t writtenPages = 0;
	
	// start JSON UART
	uart0_send_string("[");
	
	while(!finalPage)
	{
		uart0_send_string("{");
		
		readPage(pages);
		
		convert_page_to_String(readData);
		
		uart0_send_string(preparedData);
		
		uart0_send_string("}");
		
		writtenPages = ((readData[0] << 16) | (readData[1] << 8) | (readData[2] << 0));
		
		if(writtenPages == 0)
		{
			finalPage = false;
		}
		
		pages++; // Go to next page in EEPROM
	}
	
	// END JSON UART
	uart0_send_string("]");
}
