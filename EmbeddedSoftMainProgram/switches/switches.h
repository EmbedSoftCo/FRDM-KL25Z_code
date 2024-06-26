/*! ***************************************************************************
 *
 * \brief     Low level driver for the switches
 * \file      switches.h
 * \author    Hugo Arends
 * \date      July 2021
 *
 * \copyright 2021 HAN University of Applied Sciences. All Rights Reserved.
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
#ifndef SWITCHES_H
#define SWITCHES_H

#include <MKL25Z4.h>
#include <stdbool.h>

// External variables
extern volatile bool sw_clicked_flag_1;
extern volatile bool sw_clicked_flag_2;
extern volatile bool sw_clicked_flag_3;
extern volatile bool sw_clicked_flag_4;

/// The number of keys available
#define N_KEYS (4)

/// Defines the type for the keys
typedef enum
{
    KEY_LEFT = 3,
    KEY_DOWN = 4,
    KEY_UP = 6,
    KEY_CENTER = 0,
		KEY_RIGHT = 5
} key_t;

// Function prototypes
void sw_init(void);
bool sw_pressed(const key_t key); 

#endif // SWITCHES_H
