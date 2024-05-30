/*! ***************************************************************************
 *
 * \brief     BME 280 framework
 * \file      bme280.c
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
#include "bme280.h"
#include "i2c1.h"

/* ==================== CALIBRATION VALUES ==================== */

	static uint16_t dig_T1 = 0x000000;	///< Temperature compensation value
  static int16_t dig_T2	 = 0x000000;	///< Temperature compensation value
  static int16_t dig_T3  = 0x000000;	///< Temperature compensation value
	
	static uint8_t dig_H1 = 0x00; 			///< humidity compensation value
  static int16_t dig_H2 = 0x0000; 		///< humidity compensation value
  static uint8_t dig_H3 = 0x00; 			///< humidity compensation value
  static int16_t dig_H4 = 0x0000; 		///< humidity compensation value
  static int16_t dig_H5 = 0x0000; 		///< humidity compensation value
  static uint8_t dig_H6 = 0x00;  			///< humidity compensation value
	
	static uint32_t t_fine = 0x0000; 		/// For the calucation
	
/**********************************************************
 *                                                        *
 * @brief Initialise the I2C and checks for BME280        *
 * @returns True on successful initialization             *
 *          False on failed read or wrong device adress   *
 *                                                        *
 **********************************************************/
bool bme280_init(void)
{
    uint8_t chipId = 0x00;
    bool read_succes = false;
		bool config_succes = false;
	 
		// Startup the I2C procotol
    i2c1_init();

		// Read the sensor ID
		// Test communication to sensor and checks that sensor is indeed BME280
    read_succes = i2c1_read(deviceAdress, CHIP_ID_ADDR , &chipId, 1);

    if(chipId != CHIP_ID || read_succes != true)
    {
        // Not a BME280 or wrong device adress
        // Or failed to read register
        return false;
    }
		
		// Configure the BME 280,
		// Returns false if failed
		config_succes = configure_bme280();
		
		if(config_succes != true)
		{
			return false;
		}
		
    return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**********************************************************
 *                                                   			*
 * @brief Reset BM280, so it will recalibrate the sensor  *
 * @return True on successful reset and calibration,	 	  *
 *         False when failed                         			*
 *																												*
 **********************************************************/
bool reset_bme280(void)
{
	uint8_t rw_i2c_succes = 0x00;
	
	rw_i2c_succes = i2c1_write(deviceAdress, SOFTRESET_BME, &RESET_COMMAND, 1);
	if(rw_i2c_succes != 0x1)
	{	
			return false;
	}
	
	// Timeout from I2C1 library
	// to prevent a infinite loop
	uint32_t TIMEOUT = I2C_TIMEOUT;
	while(read_calibration_status()) // read F3
	{
		if(--TIMEOUT == 0)
    {
			return false; // Calibration failed
    }
	}
		
	return true; // Reset and calibration succes
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*****************************************************
 *                                                   *
 * @brief Sets the default configuration for BME280  *
 * @return True when default configuration is set,   *
 *         False when failed                         *
 *****************************************************/
bool configure_bme280(void)
{
    uint8_t rw_i2c_succes = 0x00;
		 
////////////////////////////////////////////////////////////////////////////
// 							RESET BME280		 
    rw_i2c_succes = reset_bme280();
    if(rw_i2c_succes != I2C_RW_SUCCES)
    { 
        return false;
    }
       
////////////////////////////////////////////////////////////////////////////	
//			CONFIGURE BME 280
		
		// Set BME 280 in sleep mode, otherwise writes will be ignored see DS 7.4.6
    rw_i2c_succes = i2c1_write(deviceAdress, CTRL_MEAS_ADDR, &SLEEP_COMMAND, 1);
    if(rw_i2c_succes != I2C_RW_SUCCES)
    { 
        return false;
    }

    rw_i2c_succes = i2c1_write(deviceAdress, CTRL_HUM_ADDR, &hum_config_reg, 1);
    if(rw_i2c_succes != I2C_RW_SUCCES)
    { 
        return false;
    }
		
    rw_i2c_succes = i2c1_write(deviceAdress, CONFIG_ADDR, &config_reg, 1);
    if(rw_i2c_succes != I2C_RW_SUCCES)
    { 
        return false;
    }

	
    rw_i2c_succes = i2c1_write(deviceAdress, CTRL_MEAS_ADDR, &ctrl_meas_reg, 1);
    if(rw_i2c_succes != I2C_RW_SUCCES)
    { 
        return false;
    }
		
		rw_i2c_succes = get_calibration();
    if(rw_i2c_succes != I2C_RW_SUCCES)
    { 
        return false;
    }
		
    return true; // Set default configuration is successful
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**************************************************************
 *                                                            *
 * @brief Read calibration register on BME280                 *
 *        This means that the sensor is busy with calibration *
 * @return True when sensor is busy with calibration          *
 *                                                            *
 **************************************************************/
bool read_calibration_status(void)
{
	uint8_t CALIBRATION_VALUE	= 0x00;
  i2c1_read(deviceAdress, REGISTER_STATUS, &CALIBRATION_VALUE, 1);
	
		return ( (CALIBRATION_VALUE & (1 << 0)) != 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*************************************************
 *                                               *
 * @brief Read calibration registers on BME280   *
 *                                               *
 *************************************************/
bool get_calibration(void)
{
	uint8_t i2c_read_succes = false;
	
	// Arrays for temporary storage of calibration values
	uint8_t dig_t1_local[2];
	uint8_t dig_t2_local[2];
	uint8_t dig_t3_local[2];
	
	uint8_t dig_h1_local[1];
	uint8_t dig_h2_local[2]; 
	uint8_t dig_h3_local[1];
	uint8_t dig_h4_local[2];
	uint8_t dig_h5_local[3];
	uint8_t	dig_h6_local[2];
	
	// Read the data from BME 280
	i2c_read_succes = i2c1_read(deviceAdress, DIG_T1_ADDR, dig_t1_local, 2);
	i2c_read_succes = i2c1_read(deviceAdress, DIG_T2_ADDR, dig_t2_local, 2);
	i2c_read_succes = i2c1_read(deviceAdress, DIG_T3_ADDR, dig_t3_local, 2);
	
	i2c_read_succes = i2c1_read(deviceAdress, DIG_H1_ADDR, dig_h1_local, 1);
	i2c_read_succes = i2c1_read(deviceAdress, DIG_H2_ADDR, dig_h2_local, 2);
	i2c_read_succes = i2c1_read(deviceAdress, DIG_H3_ADDR, dig_h3_local, 1);
	i2c_read_succes = i2c1_read(deviceAdress, DIG_H4_ADDR, dig_h4_local, 2);
	i2c_read_succes = i2c1_read(deviceAdress, DIG_H5_ADDR, dig_h5_local, 2);
	i2c_read_succes = i2c1_read(deviceAdress, DIG_H6_ADDR, dig_h6_local, 1);
	
	if(i2c_read_succes != I2C_RW_SUCCES) 
	{	
		return false; // read failed
	}
	
	// Store BME 280  data in the calibration
	dig_T1 = (int16_t)((dig_t1_local[1] << 8) | (dig_t1_local[0] << 0));
	dig_T2 = (int16_t)((dig_t2_local[1] << 8) | (dig_t2_local[0] << 0));
	dig_T3 = (int16_t)((dig_t3_local[1] << 8) | (dig_t3_local[0] << 0));
	
	dig_H1 = dig_h1_local[0];	
  dig_H2 = (int16_t)((dig_h2_local[1] << 8) | (dig_h2_local[0] << 0));	
  dig_H3 = dig_h3_local[0];	
  dig_H4 = (int16_t)((dig_h4_local[0] << 4) | (dig_h4_local[1] & 0xF));
  dig_H5 = (int16_t)((dig_h5_local[1] << 4) | (dig_h5_local[0] >> 4));	
  dig_H6 = dig_h6_local[0]; 
	
	return true; // Read and save calibration data is successful
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*************************************************
 *                                               *
 * @brief Read temperature registers on BME280   *
 * @returns returns the temperature in 4 digits, *
 *					so "5123" is 51.23 degC							 *
 *                                               *
 *************************************************/
uint32_t get_temperature(void)
{
	bool i2c_read_succes = false;
	uint8_t temperature[3];
	
	int32_t var1 	=  0x0000;
	int32_t var2 	=  0x0000;
	int32_t adc_T =  0x0000;
	int32_t T 		=  0x0000;
	
	i2c1_write(deviceAdress, CTRL_MEAS_ADDR, &ctrl_meas_reg, 1); // start the sampling
	
	i2c_read_succes = i2c1_read(deviceAdress, TEMP_ADDR, temperature, 3);
	if(i2c_read_succes != I2C_RW_SUCCES)
    { 
        return 0x00; // Read failed
    }
		
		// Store temperature array in one 32 bits register
		adc_T = ((temperature[0] << 16) | (temperature[1] << 8) | (temperature[2] << 0));
		adc_T = adc_T >> 4; // Rightshift to remove 0 in bits 0 to 3 of temp_xlsb. See Table 18 in Datasheet
		
		// Formula from datasheet 10.2 (page 50)
		var1 = ((((adc_T>>3) - (dig_T1<<1))) * (dig_T2)) >> 11;
		var2 = (((((adc_T>>4) - (dig_T1)) * ((adc_T>>4) - (dig_T1))) >> 12) * (dig_T3)) >> 14;
		
		t_fine = var1 + var2;
		
		T = (t_fine * 5 + 128) >> 8;
		
		return T; // returns temperature: output value "5123" is 51.23 degC
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************************
 *                                                																		 *
 * @brief Read humidity registers on BME280 	    																		 *
 * @returns returns the humidity in Q22.10 format (22 integer and 10 fractional bits). *
 *					output is "47445" represents 47445/1024 = 46.333 %RH										   *
 *                                             																			   *
 ***************************************************************************************/
uint32_t get_humidity(void)
{
	bool i2c_read_succes = false;
	uint8_t humidity[2];
	
	int32_t v_x1_u32r = 0x0000;
	int32_t adc_H = 0x0000;
	
	i2c1_write(deviceAdress, CTRL_MEAS_ADDR, &ctrl_meas_reg, 1); // start the sampling
	
	i2c_read_succes = i2c1_read(deviceAdress, TEMP_ADDR, humidity, 2);
	if(i2c_read_succes != I2C_RW_SUCCES)
    { 
        return 0x00; // Read failed
    }
		
		// Store humidity array in one 32 bits register
		adc_H = ((humidity[0] << 8) | (humidity[1] << 0));
		
		// Formula from datasheet 6.2.3 (page 23 and 24)
		v_x1_u32r = (t_fine - ((int32_t)76800));
		v_x1_u32r = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * v_x1_u32r)) +
								((int32_t)16384)) >> 15) * (((((((v_x1_u32r * ((int32_t)dig_H6)) >> 10) * (((v_x1_u32r *
								((int32_t)dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) *
								((int32_t)dig_H2) + 8192) >> 14));
		
		v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)dig_H1)) >> 4));
		v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
		v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
	
		return (uint32_t)(((float)(v_x1_u32r >> 12)/1024)*100); // Returns humidity: output value "47445" is 47445/1024 = 46.333 %RH
}
// End of BME280.c
