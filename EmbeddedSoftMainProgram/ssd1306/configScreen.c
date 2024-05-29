#include "configScreen.h"

/*!
 * \brief Check if a switch is pressed
 *
 * This function place the arrow on the line before the answer.
 *
 * \param[in] Line of selected choice. Could be 1, 2, 3.
 */
void setSelect(int line){
	
		switch(line)
		{
			case 1:
				ssd1306_putstring(0,24,">");
				ssd1306_putstring(0,36," ");
				ssd1306_putstring(0,48," ");
				ssd1306_update();
				break;
			case 2:
				ssd1306_putstring(0,24," ");
				ssd1306_putstring(0,36,">");
				ssd1306_putstring(0,48," ");
				ssd1306_update();
				break;
			case 3:
				ssd1306_putstring(0,24," ");
				ssd1306_putstring(0,36," ");
				ssd1306_putstring(0,48,">");
				ssd1306_update();
				break;
		}
		
}

/*!
 * \brief Get the selection from user
 *
 * This function can be user after functione displayPuzzle to get the user selction input and display the slection arrow.
 * Continue this function by chosing one of the choice.
 * 
 * \return the selected line. Could be 1, 2, 3.
 */
int getSelection(void){
	int selection = 0;
	int select = 1;
		
	while(selection == 0){
		
		if(sw_pressed(KEY_UP)){
			select--;
		}
		if(sw_pressed(KEY_DOWN)){
			select++;
		}
		
		if(select > 3){
			select = 1;
		}else if(select < 1){
			select = 3;
		}
		
		setSelect(select);
		
		if(sw_pressed(KEY_CENTER)){
			
		switch(select){
			case 1:
				selection = 1;
				break;
			case 2:
				selection = 2;
				break;
			case 3:
				selection = 3;
				break;
		}
	}
	}
	
	ssd1306_clearscreen();
	ssd1306_update();
	
	return selection;
}

void displayCorrect(void)
{
		ssd1306_clearscreen();
    ssd1306_setfont(Dialog_plain_12);
    ssd1306_putstring(0,0,"Correct!");
		ssd1306_update();	
		delay_us(1000000);
}
	
void displayWrong(void)
{
		ssd1306_clearscreen();
    ssd1306_setfont(Dialog_plain_12);
    ssd1306_putstring(0,0,"Wrong!");
		ssd1306_update();	
		delay_us(1000000);
}
	
/*!
 * \brief Show starting display
 *
 * Use this funciton to start the game.
 * This functions ask user to click on one of the buttons to continue the program.
 * 
 */
void displayStart(void){
		
		sw_init();
		ssd1306_init();
		ssd1306_setorientation(1);   
    ssd1306_clearscreen();
    ssd1306_setfont(Dialog_plain_12);
    ssd1306_putstring(0,0,"Reverse Geocache");
    ssd1306_setfont(Monospaced_plain_10);
		ssd1306_putstring(10,50,"Program starting"); 
		ssd1306_update();		
}

/*!
 * \brief Display the information.
 *
 * This function display the distance to the location and played time and a completed locations and the temperature.
 * 
 * \todo This funciton still using the constant information. It could be global variable(string).
 */
void displayDistance(const char *distance, const char *location, const char *temp, const char *hum) {
		ssd1306_clearscreen();	
		ssd1306_setfont(Dialog_plain_12);
		ssd1306_putstring(0,0,"Distance: ");
		ssd1306_putstring(0,12,"Time: ");
		ssd1306_putstring(0,24,"Location:");
		ssd1306_putstring(0,36,"Temp: ");
		ssd1306_putstring(0,48,"Hum: ");
		ssd1306_putstring(60,0, distance);
		ssd1306_putstring(60,12,"00:01");
		ssd1306_putstring(60,24,location);
		ssd1306_putstring(60,36, temp);
		ssd1306_putstring(100,36," C");
		ssd1306_putstring(60,48, hum);
		ssd1306_putstring(105,48," %");
		ssd1306_update();
}

/*!
 * \brief Display quetion and answers
 *
 * This function display the quetion and answers when the user are at the certain location.
 * 
 * \param[in] 	1. Quetion
 * \param[in]		2. First answer
 * \param[in]		3. Second answer
 * \param[in]		4. Third answer
 *
* \return bool: true is correct answer and false is incorrect answer.
 */
bool displayPuzzle(const char *aPuzzle, const char *aAnswer_1, const char *aAnswer_2, const char *aAnswer_3, const int *goodAnswer){
			
		bool result = false;
		int answer;
	
		ssd1306_clearscreen();
		ssd1306_setfont(Monospaced_plain_10);
		ssd1306_putstring(0,0,aPuzzle);
    ssd1306_putstring(10,24,aAnswer_1); 
		ssd1306_putstring(10,36,aAnswer_2);
		ssd1306_putstring(10,48,aAnswer_3);    			
    ssd1306_update();
	
		answer = getSelection();
		
		if(answer == *goodAnswer)
		{
			result = true;
			displayCorrect();
		}
		else
		{
			result = false;
			displayWrong();
		}
	
		return result;
}

