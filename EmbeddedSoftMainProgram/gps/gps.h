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
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    char *formatted_time[30];
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
	char *answer[4][30];
	int goodAnswer;
} gameLocation_t;



void gps_init(void);

//checks if there is any new data
dataGps_t gps_newData(void);

//returns dataGps_t struct with info
dataGps_t gps_getData(void);

//calculates the distance between 2 points and gives back the distance in meters
double gps_calculateDistance(point_t point1, point_t point2);

dateTime_t convert_unix_timestamp(time_t unix_timestamp);

#endif
