#ifndef DISPLAY_H_
#define DISPLAY_H_
#include "msp.h"
#include "UART.h"
#include "delay.h"
#include <math.h>
#include <stdio.h>

#define clear "[2J"       //VT100 cmd to clear whole screen
#define home "[H"         //VT100 cmd to home the cursor home
#define clearLine "[2k"        //Clears the whole line
// Description: for the menu of things
void Menu();
// Description: prints out the freq
void sendFreq (unsigned int temp);

// Description: makes a bar graph for a given voltage
void barGraph(float voltage);

//Description: prints values in their respective spots on the terminal
void getValues(float RMS, float Vpp, unsigned int freq);

#endif
