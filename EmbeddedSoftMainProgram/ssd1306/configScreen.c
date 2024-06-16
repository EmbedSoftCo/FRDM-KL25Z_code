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
int getSelection(int amountAnswers){
	int selection = 0;
	int select = 1;
		
	while(selection == 0){
		
		if(sw_pressed(KEY_UP)){
			select--;
		}
		if(sw_pressed(KEY_DOWN)){
			select++;
		}
		
		if(select > amountAnswers){
			select = 1;
		}else if(select < 1){
			select = amountAnswers;
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

/*!
 * \brief Initialize the display
 *
 * 
 * 
 */
void displayInit(void)
{	
		sw_init();
		ssd1306_init();
		ssd1306_setorientation(1);   
    ssd1306_clearscreen();
    ssd1306_setfont(Monospaced_plain_10);
		ssd1306_putstring(0,0,"WELKOM");
		ssd1306_putstring(10,24, "Starting..."); 
		ssd1306_update();
}

/*!
 * \brief Show starting display
 *
 * Use this funciton to select the microcontroller mode: admin or user.
 * This function ask user to click on one of the buttons to continue the program.
 * 
 * \return Bool of with mode is selected
 */
bool displayStart(void)
{	
		int answer = false;
		bool result = false;
    ssd1306_clearscreen();
    ssd1306_setfont(Monospaced_plain_10);
		ssd1306_putstring(0,0,"SELECT USER:");
		ssd1306_putstring(10,24, "Admin"); 
		ssd1306_putstring(10,36, "User");
		ssd1306_update();
	
		answer = getSelection(2);
		if(answer == 1)
		{
			result = false;
		}
		else if(answer == 2)
		{
			result = true;
		}
		return result;
}

/*!
 * \brief Display the information.
 *
 * This function display the distance to the location and played time and a completed locations and the temperature.
 * 
 * \todo This funciton still using the constant information. It could be global variable(string).
 */
void displayDistance(const char *distance, const char *time, const char *temp, const char *hum) 
{
		ssd1306_clearscreen();	
		ssd1306_setfont(Monospaced_plain_10);
		ssd1306_putstring(0,0,"FIND THE LOCATION");
		ssd1306_putstring(0,12,"Distance: ");
		ssd1306_putstring(0,24,"Time: ");
		ssd1306_putstring(0,36,"Temp: ");
		ssd1306_putstring(0,48,"Hum: ");
		ssd1306_putstring(60,12,distance);
		ssd1306_putstring(110,12," m");
		ssd1306_putstring(60,24, time);
		ssd1306_putstring(60,36, temp);
		ssd1306_putstring(110,36," C");
		ssd1306_putstring(60,48, hum);
		ssd1306_putstring(110,48," %");
		ssd1306_update();
}

/*!
 * \brief Display question and answers
 *
 * This function display the quetion and answers when the user are at the certain location.
 * 
 * \param[in] 	1. Question
 * \param[in]		2. First answer
 * \param[in]		3. Second answer
 * \param[in]		4. Third answer
 *
* \return bool: true is correct answer and false is incorrect answer.
 */
bool displayPuzzle(const char *aPuzzle, const char *aAnswer_1, const char *aAnswer_2, const char *aAnswer_3, volatile int *goodAnswer){
			
		bool result = false;
		int answer;
	
		ssd1306_clearscreen();
		ssd1306_setfont(Monospaced_plain_10);
		ssd1306_putstring(0,0,aPuzzle);
    ssd1306_putstring(10,24,aAnswer_1); 
		ssd1306_putstring(10,36,aAnswer_2);
		ssd1306_putstring(10,48,aAnswer_3);    			
    ssd1306_update();
	
		answer = getSelection(3);
		
		if(answer == *goodAnswer)
		{
			result = true;
		}
		else
		{
			result = false;
		}
	
		return result;
}

/*!
 * \brief Display show text
 *
 * This function display the inserted text in the function
 * 
* \param[in] 	text: string of the text that will be displayed.
 *
* \return nothing
 */
void displayShowText(const char *title, const char *text)
{
		ssd1306_clearscreen();
    ssd1306_setfont(Monospaced_plain_10);
    ssd1306_putstring(0,0,title);
    ssd1306_putstring(0,24,text);
		ssd1306_update();	
}

