#ifndef NMEA_H
#define NMEA_H

#include <stdbool.h>
#include <stdint.h>

void parse_nmea(const char *nmea_data);
void parse_single_nmea(char *nmea_data);
#endif  // !NMEA_H
