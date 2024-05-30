#include "logging.h"
#include "EEPROM.h"
#include "bme280.h"
#include "gps.h"
#include <MKL25Z4.h>
#include <stdint.h>

#define RECORDS_OFFSET (1)

extern volatile bool logFlag;
static struct LogData PageBuffer[32] = {0};
static uint32_t numOfRecords = 0;
static void logInfo(void);
extern uint8_t finishedRoute;
static uint8_t page = 0x0; // Page 0 = 0x0 	// Page 7 = E00

/**
 * @brief call regularly to fill the buffer
 */
void PerodicLogging(void) {

  uint32_t logTemp32 = get_temperature(); // Get temperature reading
  uint32_t logHum32 = get_humidity();     // Get humidity reading
  dataGps_t gpsdat = gps_getData();
  // GET GPS DATA HERE

  struct LogData tmpbuf = {.lattitude = gpsdat.loc.lat,  // TODO: actual data
                           .longtitude = gpsdat.loc.lon, // TODO: actual data
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

  struct MetaData md = {.countOfRecords = numOfRecords};

  PageBuffer[0] = *(struct LogData *)&md; //* ( long * ) &

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
