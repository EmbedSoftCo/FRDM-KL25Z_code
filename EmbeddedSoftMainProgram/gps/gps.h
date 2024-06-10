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

typedef struct DateTime{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} dateTime_t;

typedef struct DataGps {
    point_t loc;
    uint32_t utc;
    enum StateGps state;
    uint8_t accuracy;
} dataGps_t;

typedef struct GameLocation {
  point_t location;
	char *question[30];
	char *answer[4][20];
	int goodAnswer;
} gameLocation_t;


//Function prototypes
void gps_init(void);
dataGps_t gps_newData(void);
dataGps_t gps_getData(void);
double gps_calculateDistance(point_t point1, point_t point2);
dateTime_t convert_unix_timestamp(uint32_t unix_timestamp);

#endif
