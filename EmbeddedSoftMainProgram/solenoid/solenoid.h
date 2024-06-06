/*! ***************************************************************************
 *
 * \brief     Low level driver for the solenoid
 * \file      solenoid.h
 * \author    Thijs Hulshof
 * \date      June 2024
 *
 *****************************************************************************/
#ifndef SOLENOID_H
#define SOLENOID_H

#include <MKL25Z4.h>
#include "delay.h"

// For creating a mask
#define MASK(x)       (1UL << (x))

/// Define for each LED
#define PIN_SOLENOID	(0)

// Function prototypes
void solenoid_init(void);
void solenoid_trigger(void);

#endif // SOLENOID_H
