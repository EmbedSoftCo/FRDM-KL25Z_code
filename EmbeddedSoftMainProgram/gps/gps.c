#include "gps.h"

//Defines
#define MESSAGE_SIZE 80
#define START_ID 2
#define GPS_ACCURACY 2
#define EARTH_RADIUS 6371000 // Earth's radius in meters
#define PI (double)3.14159265358979323846

enum MessageType {
    GGA, ZDA, NOT_RELEVANT, UNKNOWN
};

//Variables
static dataGps_t gpsData;
static uint8_t MessageTypeStr[2][4] = {"GGA", "ZDA"};

//Prototypes
int strToInt(uint8_t * str);
float strToFloat(uint8_t * str);

/*!
 * \brief Initialize GPS
 *
 * This function initializes the uart with the GPS module is connected
 *
 * \return nothing
 */
void gps_init(void) {
    uart2_init();
}

/*!
 * \brief GPS get new data
 *
 * This function reads the data from the uart (GPS). The new data will be stored in gpsData.
 *
 * \return bool: always false. To break out the function when all needed data.
 */
dataGps_t gps_newData(void) {
    static uint8_t message[MESSAGE_SIZE];
    static uint32_t index;
    static enum MessageType messageType = UNKNOWN;
    static uint8_t contentCount = 0;
    static int32_t tempLoc = 0;
    static struct tm date;
	
		static bool checkGGA = false;
		static bool checkZDA = false;
	
   while (1) {
        message[index] = uart2_receive_poll();
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
                        tempLoc = (int32_t) ((strToFloat(message+2) * 1000000) / 60); //from minutes to degrees

                        tempLoc += (strToInt(message)/100) * 1000000; //fixed point number

                    } else if (contentCount == 3) { //North/South
                        if (message[0] == 'S') {
                            gpsData.loc.lon = -tempLoc;
                        } else {
                            gpsData.loc.lon = tempLoc;
                        }
                    } else if (contentCount == 4) {
                        tempLoc = (int32_t) ((strToFloat(message+3) * 1000000) / 60); //from minutes to degrees

                        tempLoc += (strToInt(message) / 100) * 1000000; //fixed point number

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
												checkGGA = true;
                    }
										
                    break;
                case ZDA: //In this message utc time is stored. Below we convert it to a unix timestamp
                    if (contentCount == 1) { //Hour, minute, second
                        date.tm_sec = strToInt(message + 4);
                        date.tm_min = strToInt(message + 2) / 100;
                        date.tm_hour = strToInt(message) / 10000;

                    } else if (contentCount == 2) { //Day of the month
                        date.tm_mday = (uint8_t) strToInt(message);

                    } else if (contentCount == 3) { //Month
                        date.tm_mon = (uint8_t) strToInt(message) - 1;

                    } else if (contentCount == 4) { //Year
                        date.tm_year = (uint16_t) strToInt(message) - 1900;

                        uint32_t tempUtc = mktime(&date);
                        if (tempUtc != (uint32_t) -1) { //check if the conversion is successful
                            gpsData.utc = tempUtc;
                        }
												checkZDA = true;
                    }
										if(checkGGA == true || checkZDA == true)
										{
											return gpsData;
										}
                    break;

            }
            index = 0;
            contentCount++;

        } else {
            index++;
        }
    }
}

/*!
 * \brief Get GPS data
 *
 * This function returns the GPS data.
 *
 * \return gpsData
 */
dataGps_t gps_getData(void) {
    return gpsData;
}

/*!
 * \brief calculateDistance
 *
 * This function calculates the distance between point 1 and point 2. 
 *
 * \param[in] 	point 1: for this application we use point 1 as the goal location
 * \param[in] 	point 2: for this application we use point 2 as the current location of GPS
 *
 * \return double: the distance between point 1 and point 2 in meters.
 */
double gps_calculateDistance(point_t point1, point_t point2) {
	
		const double R = 6371.0; // Earth radius in kilometers
    const double P = PI / 180.0; // Conversion factor
	  double lat1 = (double) (point1.lat / 1000000.0);
    double lon1 = (double) (point1.lon / 1000000.0);
    double lat2 = (double) (point2.lat / 1000000.0);
    double lon2 = (double) (point2.lon / 1000000.0);
 
    double a = 0.5 - cos((lat2 - lat1) * P) / 2 +
               cos(lat1 * P) * cos(lat2 * P) *
               (1 - cos((lon2 - lon1) * P)) / 2;
 
    return (double)(2000 * R * asin(sqrt(a)));
}

dateTime_t convert_unix_timestamp(time_t unix_timestamp) {
    volatile struct tm *dt;
    volatile dateTime_t result;
    
    // Convert the Unix timestamp to a struct tm
    dt = localtime(&unix_timestamp);
    
    // Extract the components
    result.year = dt->tm_year + 1900;
    result.month = dt->tm_mon + 1;
    result.day = dt->tm_mday;
    result.hour = dt->tm_hour;
    result.minute = dt->tm_min;
    result.second = dt->tm_sec;
    
    // Format the date and time into a string
		sprintf(*result.formatted_time, "%d:%d:%d", result.hour, result.minute, result.second);
    return result;
}

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
