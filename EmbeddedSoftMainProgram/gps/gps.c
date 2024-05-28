///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//          Include
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>

#include "gps.h"
#include "uart2.h"
#include "queue.h"
#include "uart0.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//          local vars / typedefs
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MESSAGE_SIZE 80
#define START_ID 2

#define GPS_ACCURACY 2

#define EARTH_RADIUS 6371000 // Earth's radius in meters

#define PI_2 (float) 1.5707963267

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//          local vars / typedefs
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum MessageType {
    GGA, ZDA, NOT_RELEVANT, UNKNOWN
};

static dataGps_t gpsData;

static uint8_t MessageTypeStr[2][4] = {"GGA", "ZDA"};

static queue_t GPS_RxQ;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//          local prototypes
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int strToInt(uint8_t * str);
float strToFloat(uint8_t * str);
float deg2rad(float deg);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//          global function
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void gps_init(void) {
    q_init(&GPS_RxQ);
    uart2_init();
}

void gps_getBuffer(queue_t **pointer) {
    *pointer = &GPS_RxQ;
}


bool gps_newData(void) {
    static uint8_t message[MESSAGE_SIZE];
    static uint32_t index;
    static enum MessageType messageType = UNKNOWN;
    static uint8_t contentCount = 0;

    static int32_t tempLoc = 0;

    static struct tm date;

    bool newData = false;


   while (1) {
        message[index] = uart2_receive_poll();
				uart0_put_char(message[index]);
        if (message[index] == '$') {
            index = 0;
            messageType = UNKNOWN;
            contentCount = 0;

        } else if (message[index] == ',' || message[index] == '*') {
            message[index] = 0;

            switch (messageType) {
                case NOT_RELEVANT:
                    break;
                case UNKNOWN:
                    messageType = NOT_RELEVANT;

                    if (contentCount == 0) {

                        for (uint8_t type = 0; type < (uint8_t) sizeof(MessageTypeStr); ++type) {
                            if (strcmp((char *) message + START_ID, (char *) MessageTypeStr[type]) == 0) {

                                messageType = (enum MessageType) type;
                            }
                        }
                    }
                    break;
                case GGA:
                    if (contentCount == 2) { //Longitude/Latitude
                        tempLoc = (int32_t) ((strToFloat(message+2) * 100000) / 60); //from minutes to degrees

                        tempLoc += (strToInt(message)/100) * 100000; //fixed point number

                    } else if (contentCount == 3) { //North/South
                        if (message[0] == 'S') {
                            gpsData.loc.lon = -tempLoc;
                        } else {
                            gpsData.loc.lon = tempLoc;
                        }
                    } else if (contentCount == 4) {
                        tempLoc = (int32_t) ((strToFloat(message+3) * 100000) / 60); //from minutes to degrees

                        tempLoc += (strToInt(message) / 100) * 100000; //fixed point number

                    } else if (contentCount == 5) { //West/East
                        if (message[0] == 'W') {
                            gpsData.loc.lat = -tempLoc;
                        } else {
                            gpsData.loc.lat = tempLoc;
                        }
                    } else if (contentCount == 6) { //Fix
                        gpsData.state = message[0];

                    } else if (contentCount == 8) { //Accuracy
                        if (gpsData.state == FIX || gpsData.state == GUESSING) {
                            gpsData.accuracy = (uint8_t) (strtol((char *) message, NULL, 10) * GPS_ACCURACY);

                        } else {
                            gpsData.accuracy = -1;
                        }
                        newData = true;
                    }
                    break;
                case ZDA: //In this message utc time is stored. Below we convert it to a unix timestamp
                    if (contentCount == 1) { //Hour, minute, second
                        date.tm_sec = strToInt(message + 4);
                        date.tm_min = strToInt(message + 2) / 100;
                        date.tm_hour = strToInt(message) / 10000;

                    } else if (contentCount == 2) { //Day of the month
                        date.tm_mday = (uint8_t) strToInt(message);

                    } else if (contentCount == 3) {    //Month
                        date.tm_mon = (uint8_t) strToInt(message) - 1;

                    } else if (contentCount == 4) { //Year
                        date.tm_year = (uint16_t) strToInt(message) - 1900;

                        uint32_t tempUtc = mktime(&date);
                        if (tempUtc != (uint32_t) -1) { //check if the conversion is successful
                            gpsData.utc = tempUtc;
                        }
                    }
										return 0;
                    

            }
            index = 0;
            contentCount++;

        } else {
            index++;
        }
    }
    return newData;
}

dataGps_t gps_getData(void) {
    return gpsData;
}


uint16_t gps_calculateDistance(point_t point1, point_t point2){
    // Convert the fixed point representation back to floating point
    float lat1_f = (float) (point1.lat / 1000000.0);
    float lon1_f = (float) (point1.lon / 1000000.0);
    float lat2_f = (float) (point2.lat / 1000000.0);
    float lon2_f = (float) (point2.lon / 1000000.0);

    // Convert latitude and longitude from degrees to radians
    float lat1_rad = deg2rad(lat1_f);
    float lon1_rad = deg2rad(lon1_f);
    float lat2_rad = deg2rad(lat2_f);
    float lon2_rad = deg2rad(lon2_f);

    // Haversine formula
    float dlon = lon2_rad - lon1_rad;
    float dlat = lat2_rad - lat1_rad;

    float a = powf( sinf(dlat / 2), 2) + cosf(lat1_rad) * cosf(lat2_rad) * powf(sinf(dlon / 2), 2);
    float c = 2 * atan2f(sqrtf(a), sqrtf(1 - a));

    // Calculate the distance in meters
    return (uint16_t) (EARTH_RADIUS * c);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//          local function
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int strToInt(uint8_t * str){
    if (str[0] == '-'){
        return 0;
    }
    return strtol((char *) str, NULL, 10);
}

float strToFloat(uint8_t * str){
    if (str[0] == '-'){
        return 3;
    }
    return strtof((char *) str, NULL);
}

float deg2rad(float deg) {
    return deg * PI_2;
}
