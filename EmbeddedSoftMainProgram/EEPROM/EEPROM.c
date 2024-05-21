/*! ***************************************************************************
 *
 * \file      EEPROM.c
 * \author    Thijs Hulshof
 * \date      MEI 2024
 *
 *             FRDM-KL25Z        M95P08-I
 *             ----------+      +-------+
 *                       |      |       |
 *                    3V3+------+VCC    |
 *                    GND+------+GND    |
 *                       |      |       |
 *                  PTA17+------+CS     |
 *        SPI0_MISO/PTD3 +------+MISO   |
 *         SPI0_SCK/PTD1 +------+SCK    |
 *        SPI0_MOSI/PTE2 +------+MOSI   |
 *                       |      |       |
 *             ----------+      +-------+
 *
 *
 *****************************************************************************/
#include <MKL25Z4.h>
#include "EEPROM.h"

#define SPI_TRANSFER_TIMEOUT (48000)

void spi_init(void)
{
		//Clock enable SPI0 and PORTD
		SIM->SCGC4 |= SIM_SCGC4_SPI0_MASK;
		SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
		SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	
		PORTA->PCR[17] = PORT_PCR_MUX(1);	//Set PTD0 to mux 2 [SPI0_PCS0]
		PORTD->PCR[1] = PORT_PCR_MUX(2);	//Set PTD1 to mux 2 [SPI0_SCK]
		PORTD->PCR[2] = PORT_PCR_MUX(2);	//Set PTD2 to mux 2 [SPI0_MOSI]
		PORTD->PCR[3] = PORT_PCR_MUX(2);	//Set PTD3 to mux 2 [SPIO_MISO]

    // Don't let the SPI peripheral control the CS pin. Instead, we control
    // the pin our selve.
    // PTE4 is an output pin and set CS high
    PORTA->PCR[17] = PORT_PCR_MUX(1);
    GPIOA->PDDR |= (1<<17);
    GPIOA->PSOR = (1<<17);

	
		// !!!!The EEPROM M95P08-I can go up to 80MHz. But for test phase the baudrate is set lower.!!!!
    // BaudRateDivisor = (SPPR + 1) x 2^(SPR + 1)
    //
    // BaudRate = BusClock / BaudRateDivisor
    //
    // SPPR[2:0] =  000 -> (SPPR + 1) = 1
    // SPR[3:0]  = 0000 -> 2^(SPR + 1) = 2
    //
    // BaudRateDivisor = 2 x 1 = 2
    // BaudRate = 48 MHz / 2 = 24 MHz
    SPI0->BR = SPI_BR_SPPR(0) | SPI_BR_SPR(0);

    // Setup SPI0
    //  SPIE  = 0: SPI interrupt disable
    //  SPE   = 1: SPI system enable
    //  SPTIE = 0: SPI transmit interrupt disable
    //  MSTR  = 1: SPI module configured as a master SPI device
    //  CPOL  = 1: Active-low SPI clock (idles high)
    //  CPHA  = 1: First edge on SPSCK occurs at the start of the first cycle of a data transfer
    //  SSOE  = 1: SS pin function is automatic SS output
    //  LSBFE = 0: SPI serial data transfers start with most significant bit
    SPI0->C1 = SPI_C1_SPE(1) | SPI_C1_MSTR(1) | SPI_C1_CPHA(1) |
               SPI_C1_CPOL(1) | SPI_C1_SSOE(1) | SPI_C1_LSBFE(0);

    //  SPMIE   = 0: SPI match interrupt disable
    //  TXDMAE  = 0: Transmit DMA disable
    //  MODFEN  = 1: Mode fault function enabled, master SS pin acts as the mode
    //               fault input or the slave select output
    //  BIDIROE = 0: When SPC0 is 0, BIDIROE has no meaning or effect.
    //  RXDMAE  = 0: Receive DMA disable
    //  SPISWAI = 0: SPI clocks continue to operate in wait mode
    //  SPC0    = 0: SPI uses separate pins for data input and data output (pin mode is normal).
    SPI0->C2 = SPI_C2_MODFEN(1);
}

static inline bool spi_send(const uint8_t * const data, const uint32_t len)
{
    // -------------------------------------------------------------------------
    // An SPI transfer consists of two steps
    // 1. Wait until SPTEF=1 and then write data to SPIx->D (even if there is
    //    nothing to write, do a dummy write to generate the SCK)
    // 2. Wait until SPRF=1 and then read data from SPIx->D (even if there is
    //    nothing to read, do a dummy read so data will not get lost)
    
    uint32_t cnt = 0;

    for(uint32_t i=0; i<len; ++i)
    {
        cnt = 0;
        
        // Wait until SPTEF=1
        while((SPI0->S & SPI_S_SPTEF_MASK) == 0)
        {
            if(++cnt > SPI_TRANSFER_TIMEOUT){ return false; }
        }

        // Write data to SPIx->D
        SPI0->D = data[i];

        cnt = 0;
        
        // Wait until SPRF=1
        while((SPI0->S & SPI_S_SPRF_MASK) == 0)
        {
            if(++cnt > SPI_TRANSFER_TIMEOUT){ return false; }
        }

        // Dummy read data from SPIx->D
        (void)SPI0->D;
    }

    return true;
}

static inline bool spi_recv(uint8_t * const data, const uint32_t len)
{
    // -------------------------------------------------------------------------
    // An SPI transfer consists of two steps
    // 1. Wait until SPTEF=1 and then write data to SPIx->D (even if there is
    //    nothing to write, do a dummy write to generate the SCK)
    // 2. Wait until SPRF=1 and then read data from SPIx->D (even if there is
    //    nothing to read, do a dummy read so data will not get lost)

    uint32_t cnt = 0;

    for(uint32_t i=0; i<len; ++i)
    {
        cnt = 0;

        // Wait until SPTEF=1
        while((SPI0->S & SPI_S_SPTEF_MASK) == 0)
        {
            if(++cnt > SPI_TRANSFER_TIMEOUT){ return false; }
        }
        
        // Dummy write data to SPIx->D
        SPI0->D = 0xFF;

        cnt = 0;

        // Wait until SPRF=1
        while((SPI0->S & SPI_S_SPRF_MASK) == 0)
        {
            if(++cnt > SPI_TRANSFER_TIMEOUT){ return false; }
        }        

        // Read data from SPIx->D
        data[i] = SPI0->D;
    }

    return true;
}

/*!
 * \brief Initialize SPI
 *
 * \return nothing
 */
void EEPROM_init(void)
{
    // Wait 20ms before operational mode is started
    delay_us(20000);
    spi_init();
}


/*!
 * \brief Enable or disable write mode of the EEPROM
 *
 * \param[in]  enable  enable write mode when set to true 
 *
 * \return bool--> true when function succeeded
 */
static inline bool EEPROM_write_mode(bool enable)
{
		uint8_t cmd = 0x00;
		if(enable)
		{
			cmd = WREN;
		}
		else if(!enable)
		{
			cmd = WRDI;
		}
		
    // CS low, selects the slave device
    GPIOA->PCOR = (1<<17);

    spi_send(&cmd, sizeof(cmd));

    // CS high, deselects the slave device
    GPIOA->PSOR = (1<<17);
		
		const uint8_t cmd2 = RDSR;
    // CS low, selects the slave device
    GPIOA->PCOR = (1<<17);

    spi_send(&cmd2, sizeof(cmd2));

		//Check if write mode is enabled
		uint8_t data = {0x00};
		spi_recv(&data, sizeof(data));
		
		// CS high, deselects the slave device
    GPIOA->PSOR = (1<<17);
		
		return data == 0x02;
}

/*!
 * \brief Write data to a page of the EEPROM
 *
 * \param[in] block		--> Block 0 = 0x0 	--> Block 15 = 0xF
 * \param[in] sector	--> Sector 0 = 0x0	--> Sector 15 = 0xF
 * \param[in] page		--> Page 0 = 0x0 	 	--> Page 7 = E00	
 * \param[in] data		--> data of max 512 bytes
 * \param[in] len			--> data length. 
 *
 * \return nothing
 */
void EEPROM_write_page(const uint8_t block, const uint8_t sector, const uint8_t page, uint8_t * const data, const uint32_t len)
{
		EEPROM_write_mode(true);
	
		uint8_t cmd[] = {PGWR, block, sector, page};
	
		// CS low, selects the slave device
		GPIOA->PCOR = (1<<17);

    spi_send(cmd, sizeof(cmd));
	
		spi_send(data, len);
    // CS high, deselects the slave device
    GPIOA->PSOR = (1<<17);
		
		// CS low, selects the slave device
		GPIOA->PCOR = (1<<17);
		delay_us(2500);
		EEPROM_write_mode(false);
}


/*!
 * \brief Read data from a page out of the EEPROM
 *
 * \param[in] block		--> Block 0 = 0x0 	--> Block 15 = 0xF
 * \param[in] sector	--> Sector 0 = 0x0	--> Sector 15 = 0xF
 * \param[in] page		--> Page 0 = 0x0 	 	--> Page 7 = E00	
 * \param[in] data		--> data of max 512 bytes
 * \param[in] len			--> data length. 
 *
 * \return nothing
 */
void EEPROM_read_page(const uint8_t block, const uint8_t sector, const uint8_t page, uint8_t * const data, const uint32_t len)
{	
		uint8_t cmd[] = {READ, block, sector, page};
	
		// CS low, selects the slave device
		GPIOA->PCOR = (1<<17);

    spi_send(cmd, sizeof(cmd));
		spi_recv(data, len);
		
    // CS high, deselects the slave device
    GPIOA->PSOR = (1<<17);
}




