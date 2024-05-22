/*! ***************************************************************************
 *
 * \brief     BME 280 framework
 * \file      bme280.h
 * \author    Julian Janssen
 * \date      April 2024
 *
 * \copyright 2024 Julian Janssen. All Rights Reserved.
 *            \n\n
 *            Permission is hereby granted, free of charge, to any person
 *            obtaining a copy of this software and associated documentation
 *            files (the "Software"), to deal in the Software without
 *            restriction, including without limitation the rights to use,
 *            copy, modify, merge, publish, distribute, sublicense, and/or sell
 *            copies of the Software, and to permit persons to whom the
 *            Software is furnished to do so, subject to the following
 *            conditions:
 *            \n\n
 *            The above copyright notice and this permission notice shall be
 *            included in all copies or substantial portions of the Software.
 *            \n\n
 *            THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *            EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *            OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *            NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *            HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *            WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *            FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *            OTHER DEALINGS IN THE SOFTWARE.
 *
 *****************************************************************************/
#ifndef BME280_H_
#define BME280_H_

#include <stdint.h>
#include <stdbool.h>

/* ==================== CONFIGURATION ==================== */

#define SENSOR_ADRESS 0x76

	/*
	 * bit 2,1,0	osrs_h[3]	= 001  //oversampling (x1)
	 */
#define OSRS_H 0b001

	/*
	 * bit 7,6,5	t_sb[3] 		 = 110 	// 10.0ms 
	 * bit 4,3,2	filter[3] 	 = 000  // No filter  
	 * bit 0			spi3w_en[0]  =   0 	// No SPI
	 */ 
#define T_SB 			0b110
#define FILTER 		0b000
#define SPI3W_EN	0b0

	/*
	 * osrs_t[3] = 001  temperature control register // oversampling (x1)
	 * osrs_P[3] = 001  pressure control register 	 // oversampling (x1)
	 * mode[2]   =  11 mode select chip							 // Normal mode
	 */
#define OSRS_T 	0b001
#define OSRS_P 	0b001
#define MODE		0b11
	
/* ==================== MASKS ==================== */

// One left SHIFT is needed for the I2C device adress
#define DEVICE_ADRESS_MASK(x) ( (x) << 1)  

// SHIFT operators for config register
#define TIME_STANDBY_MASK(x) ( (x) << 5) 
#define FILTER_MASK(x) ( (x) << 2) 

//SHIFT operators for control measure register
#define OSRS_T_MASK(x) ( (x) << 5) // Temperature oversampling
#define OSRS_P_MASK(x) ( (x) << 2) // Pressure oversampling

// SHIFT operator for humitity config register
#define OSRS_H_MASK(X) ( (X) << 0) // Humidity oversampling

/* ==================== FUNCIONS ==================== */

bool init_bme280(void);
bool reset_bme280(void);
bool configure_bme280(void);
bool read_calibration_status(void);

bool get_calibration(void);
int32_t get_temperature(void);

/* ==================== SETUP REGISTERS ==================== */

  // Adress of BME280
	static const uint8_t deviceAdress = DEVICE_ADRESS_MASK(SENSOR_ADRESS);  

	static const uint8_t hum_config_reg = OSRS_H;
	                               
	static const uint8_t config_reg = TIME_STANDBY_MASK(T_SB) | FILTER_MASK(FILTER) | 0b0 | SPI3W_EN;
	
	static const uint8_t ctrl_meas_reg = OSRS_T_MASK(OSRS_T) | OSRS_P_MASK(OSRS_P) | MODE;
	 

/* ==================== BME 280 ADRESSES ==================== */

   // Adresses that are read -or writtable 
   static const uint8_t CTRL_HUM_ADDR   = 0xF2;
   static const uint8_t CTRL_MEAS_ADDR  = 0xF4;
   static const uint8_t CONFIG_ADDR     = 0xF5;
   static const uint8_t PRESS_ADDR      = 0xF7;
   static const uint8_t TEMP_ADDR       = 0xFA;
   static const uint8_t HUM_ADDR        = 0xFD;
   static const uint8_t TEMP_DIG_ADDR   = 0x88;
   static const uint8_t PRESS_DIG_ADDR  = 0x8E;
   static const uint8_t HUM_DIG_ADDR1   = 0xA1;
   static const uint8_t SOFTRESET_BME		= 0XE0;
   static const uint8_t HUM_DIG_ADDR2   = 0xE1;
   static const uint8_t CHIP_ID_ADDR    = 0xD0;

	 // Writable values
	 static const uint8_t REGISTER_STATUS = 0xF3;
   static const uint8_t RESET_COMMAND   = 0xB6;
   static const uint8_t SLEEP_COMMAND   = 0b00;

   //Values to check BME 280
   static const uint8_t CHIP_ID       	= 0x60;
   static const uint8_t I2C_RW_SUCCES 	= 0x01;


	 static const uint8_t DIG_T1_ADDR = 0x88; // To 0x89
	 static const uint8_t DIG_T2_ADDR = 0x8A; // To 0x8B
	 static const uint8_t DIG_T3_ADDR = 0x8C; // To 0x8D

	 static const uint8_t DIG_P1_ADDR = 0x8E; // To 0x7F
	 static const uint8_t DIG_P2_ADDR = 0x90; // To 0x91
	 static const uint8_t DIG_P3_ADDR = 0x92; // To 0x93
	 static const uint8_t DIG_P4_ADDR = 0x94; // To 0x95
	 static const uint8_t DIG_P5_ADDR = 0x96; // To 0x97
	 static const uint8_t DIG_P6_ADDR = 0x98; // To 0x99
	 static const uint8_t DIG_P7_ADDR = 0x9A; // To 0x9B
	 static const uint8_t DIG_P8_ADDR = 0x9C; // To 0x9D
	 static const uint8_t DIG_P9_ADDR = 0x9E; // To 0x9F
	 
	 static const uint8_t DIG_H1			= 0xA1;
	 static const uint8_t DIG_H2			= 0xE1; // To 0xE2
	 static const uint8_t DIG_H3			= 0xE3;

#endif // BME280_H_
