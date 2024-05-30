	#ifndef CONFIG_SCREEN_H
#define CONFIG_SCREEN_H

#include "ssd1306.h"
#include "delay.h"
#include "switches.h"

void displayInit(void);
bool displayStart(void);
void displayDistance(const char *distance, const char *location, const char *temp, const char *hum);
bool displayPuzzle(const char *aPuzzle, const char *aAnswer_1, const char *aAnswer_2, const char *aAnswer_3, const int *goodAnswer);
void displayShowText(const char *text);

#endif // CONFIG_SCREEN_H
