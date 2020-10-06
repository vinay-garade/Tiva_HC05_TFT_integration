/*------------------------------------TM4C123 Cortex-M4F Project----------------------------------------
Version: 1.1
Author(s): Damola Awesu; Vinay Garade
File Description: Contains the main() function for the project

------------------------------------------------------------------------------------------------------*/

#include "TM4C123.h"                    // Device header
#include "bsp.h"
#include <stdint.h>

int main() {
	
	setupPeripherals();
	LCD_Init();
	
	LCD_4BitCommand(CLR_SCR);
	LCD_4BitCommand(LCD_ON);
	
	printf("Test Setup\r\n");
	
	while(1);
	
}
