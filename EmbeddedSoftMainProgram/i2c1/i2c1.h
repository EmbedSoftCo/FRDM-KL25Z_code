/*! ***************************************************************************
 *
 * \brief     I2C low level peripheral driver
 * \file      i2c0.h
 * \author    Hugo Arends
 * \date      April 2022
 *
 * \copyright 2022 HAN University of Applied Sciences. All Rights Reserved.
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
#ifndef I2C0_H
#define I2C0_H

#include <stdint.h>
#include <stdbool.h>

/*!
 * \brief Definition for the I2C timeout
 *
 * This timeout value is used in loops to wait for a bit to set/reset. If the
 * bit doesn't get set, the function returns.
 */
#define I2C_TIMEOUT (100000)

// Function prototypes
void i2c1_init(void);

bool i2c1_read(const uint8_t device_address, const uint16_t address, uint8_t data[], const uint16_t n);
bool i2c1_write(const uint8_t device_address, const uint16_t address, const uint8_t data[], const uint16_t n);
void wait_on_I2C(void);
    
#endif // I2C0_H
