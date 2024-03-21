#include "coords.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

coordinate str_to_coordinate(char* lat, char* lon, bool isNorth, bool isEast) {
    coordinate coord = {0};

    coord.x.is_north_or_east = isNorth;
    coord.y.is_north_or_east = isEast;
    char tlat[3] = {0};
    char tlon[4] = {0};
    char lond[9] = {0};
    char latd[9] = {0};

    strncpy(tlat, lat, 2);
    strncpy(tlon, lon, 3);
    strncpy(latd, lat + 2, 7);
    strncpy(lond, lon + 3, 7);
    strcpy(&latd[2], &latd[3]);
    strcpy(&lond[2], &lond[3]);

    printf("lat: %s\n", tlat);
    printf("lon: %s\n", tlon);
    printf("latd: %s\n", latd);
    printf("lond: %s\n", lond);
    coord.x.coord = atoi(tlat) * COORD_MULTIPLIER + atoi(latd) / 60;
    coord.y.coord = atoi(tlon) * COORD_MULTIPLIER + atoi(lond) / 60;

    printf(COORDINATE_Fmt, COORDINATE_Arg(coord));

    return coord;
}

