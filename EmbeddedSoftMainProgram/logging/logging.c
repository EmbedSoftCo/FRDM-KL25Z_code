#include "logging.h"
#include <MKL25Z4.h>
#include <stdio.h>
#include "EEPROM.h"
#include "bme280.h"
#include "gps.h"

extern volatile bool logFlag;
static struct LogData PageBuffer[32]={0};


void logInfo(void)
{
	uint32_t logTemp32 = 0x00000;
	uint32_t logHum32 = 0x00000;
	
	uint8_t* logTemp8;
	uint8_t* logHum8;
	
	uint8_t block = 0x0; 		// Block 0 = 0x0 	// Block 15 = 0xF
	uint8_t sector = 0x0;		// Sector 0 = 0x0 // Sector 15 = 0xF
	uint8_t page = 0x0; 		// Page 0 = 0x0 	// Page 7 = E00	
	
/* ================ Read data ================ */	
	logTemp32 = get_temperature();	// Get temperature reading
	logHum32 = get_humidity();			// Get humidity reading

	
	struct MetaData md = {.countOfRecords = 0};
	
	struct LogData tmpbuf = {	.lattitude 		= 0000,
												.longtitude 	= 0000,
												.temperature 	= logTemp32,
												.humidity 		= logHum32
											};
	
  PageBuffer[0] = *(struct LogData*) &md;//* ( long * ) &
	PageBuffer[1]= tmpbuf;
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
	// EEPROM_write_page(block, sector, page, data_write, sizeof(data_write));
	
	// Reset  the log flag
	logFlag = false;
}
