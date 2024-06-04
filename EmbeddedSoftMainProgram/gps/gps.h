#ifndef GPS_H
#define GPS_H

#include <MKL25Z4.h>
#include "queue.h"
#include "uart2.h"
#include "uart0.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>

#define MAXRADIUS 10

enum StateGps {NO_FIX = '0', FIX = '1', GUESSING = '6'};

typedef struct Point {
    int32_t lat;
    int32_t lon;
} point_t;

typedef struct DataGps {
    point_t loc;
    uint32_t utc;
    enum StateGps state;
    uint8_t accuracy;
} dataGps_t;

typedef struct GameConfig {
  point_t location[5];
	int amountLocations;
} gameConfig_t;

void gps_init(void);

//checks if there is any new data
bool gps_newData(void);

//returns dataGps_t struct with info
dataGps_t gps_getData(void);

double gps_calculateDistance(point_t point1, point_t point2);
#endif
