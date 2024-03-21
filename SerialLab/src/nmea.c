#include "nmea.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "coords.h"

char *strtok_fr(char *s, char delim, char **save_ptr) {
    char *tail;
    char c;

    if (s == NULL) {
        s = *save_ptr;
    }
    tail = s;
    if ((c = *tail) == '\0') {
        s = NULL;
    } else {
        do {
            if (c == delim) {
                *tail++ = '\0';
                break;
            }
        } while ((c = *++tail) != '\0');
    }
    *save_ptr = tail;
    return s;
}

char *strtok_f(char *s, char delim) {
    static char *save_ptr;
    return strtok_fr(s, delim, &save_ptr);
}

void parse_nmea(const char *nmea_data) {
    const char separator[] = "\r\n";
    char internal_data[2048];
    strcpy(internal_data, nmea_data);
    char *token = strtok(internal_data, separator);

    /* walk through other tokens */
    while (token != NULL) {
        parse_single_nmea(token);
        token = strtok(NULL, separator);
    }
    // #warning "Not implemented yet"
}

void parse_single_nmea(char *nmea_data) {
    char *Message_ID = strtok_f(nmea_data, ',');
    if (strcmp(Message_ID, "$GNGGA") == 0) {
        char *Time = strtok_f(NULL, ',');
        char *Raw_Latitude = strtok_f(NULL, ',');
        char *N_S = strtok_f(NULL, ',');
        char *Raw_Longitude = strtok_f(NULL, ',');
        char *E_W = strtok_f(NULL, ',');
        char *posfix = strtok_f(NULL, ',');  // should be 1 or 2
        char *COG = strtok_f(NULL, ',');     // used sattilites 0-12
        char *Date = strtok_f(NULL, ',');    // used sats hdop horisontal res
        char *Magnetic_Variation = strtok_f(NULL, ',');  // msl altitude in
        char *M_E_W = strtok_f(NULL, ',');               // meters? char
        char *Positioning_Mode = strtok_f(NULL, ',');    // separation
        printf("Message ID: %s\n", Message_ID);
        printf("Time: %s\n", Time);
        printf("Raw Latitude: %s\n", Raw_Latitude);
        printf("N/S: %s\n", N_S);
        printf("Raw Longitude: %s\n", Raw_Longitude);
        printf("E/W: %s\n", E_W);
        printf("posfix: %s\n", posfix);
        printf("hdop: %s\n", COG);
        printf("msl: %s\n", Date);
        printf("Magnetic Variation: %s\n", Magnetic_Variation);
        printf("M/E/W: %s\n", M_E_W);
        printf("Positioning Mode: %s\n", Positioning_Mode);

        coordinate coord = str_to_coordinate(Raw_Latitude, Raw_Longitude,
                                             N_S[0] == 'N', E_W[0] == 'E');
    }
}
