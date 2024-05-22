#ifndef EEPROM_H
#define EEPROM_H

#include <MKL25Z4.h>
#include <stdbool.h>
#include <delay.h>

#include <stdint.h>

// ----------------------------------------------------------------------------
enum instructions { 
WREN = 0x06, //Write enable
WRDI = 0x04, //Write disable
RDSR = 0x05, //Read status register
WRSR = 0x01, //Write status register
READ = 0x03, //Read data single output
PGWR = 0x02, //Page write (erase and program)
PGER = 0xDB, //Page erase (512 bytes)
SCER = 0x20, //Sector erase (4Kbytes)
BKER = 0xD8, //Block erase (64Kbytes)
CHER = 0xC7  //Chip erase
};



// Function prototypes
void EEPROM_init(void);
void EEPROM_write_page(const uint8_t block, const uint8_t sector, const uint8_t page, uint8_t * const data, const uint32_t len);
void EEPROM_read_page(const uint8_t block, const uint8_t sector, const uint8_t page, uint8_t * const data, const uint32_t len);

#endif // EEPROM.h
