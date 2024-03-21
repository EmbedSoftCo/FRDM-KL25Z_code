#ifndef COORDS_H
#define COORDS_H
#include <stdbool.h>
#include <stdint.h>

#define COORD_MULTIPLIER 10000000  // this makes all coordinates integer numbers

#define COORDINATE_Fmt "x:%u,y:%u in Decimal Degrees"
#define COORDINATE_Arg(c) (c).x.coord, (c).y.coord

typedef struct {
    bool is_north_or_east;
    uint32_t coord;
} point;

typedef struct {
    point x;
    point y;
} coordinate;

coordinate str_to_coordinate(char* lat, char* lon, bool isNorth, bool isEast);

#endif  // !COORDS_H
