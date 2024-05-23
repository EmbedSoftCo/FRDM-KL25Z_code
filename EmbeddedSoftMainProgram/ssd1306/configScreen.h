	#ifndef CONFIG_SCREEN_H
#define CONFIG_SCREEN_H

#include "ssd1306.h"
#include "delay.h"
#include "switches.h"

void setSelect(int line);
void displayStart(void);
void displayDistance(const char *distance, const char *location, const char *temp);
void displayPuzzle(const char *aPuzzle, const char *aAnswer_1, const char *aAnswer_2, const char *aAnswer_3);
int getSelection(void);


#endif // CONFIG_SCREEN_H
