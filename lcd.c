/*------------------------------------LCD Module-------------------------------------------------------
Version: 1.0
Author(s): Damola Awesu; Vinay Garade
File Description: Header File containing function definitions for LCD control used in files: main.c, lcd.c

------------------------------------------------------------------------------------------------------*/

#include "TM4C123.h"                    // Device header
#include "bsp.h"
#include <string.h>

void LCD_Init(void) {
	/*Initialise PA5 - PA7 as LCD command pins RS, RW, E; Initialise PB0 - PB7 as LCD Data Pins D0 - D7*/
	
	GPIOA->DIR |= CMD_CFG;						
	GPIOA->DEN |= CMD_CFG;
	GPIOC->DIR = DATA_CFG;
	GPIOC->DEN = DATA_CFG;
	/*Configure LCD for Startup*/
	LCD_4BitCommand(MODE_4BIT);
	LCD_4BitCommand(CURS_HOME);
	LCD_4BitCommand(CURSOR_SHIFT);
	LCD_4BitCommand(LCD_ON);
	LCD_4BitCommand(CLR_SCR);
}

void LCD_4BitTransfer(unsigned char data, unsigned char reg_select) {
	/*
	This function converts 8-bit input(data or command) into 4 bits for writing on the LCD data pins D4-D7.
	Parameter reg_select can have values 0(for command) or 1(for data)
	*/
	data &= 0xF0;
	reg_select &= 0x0F;
	GPIOA->DATA = (reg_select << 5);																	//set RS to reg_select
	GPIOC->DATA = data;
	GPIOA->DATA |= (reg_select << 5)|(1 << 7);												//set EN to HIGH
	delay_us(0);
	GPIOA->DATA &= ~(1 << 7);																					//disable EN
}


void LCD_4BitCommand(unsigned char command) {
	/*
		This function takes an 8-bit variable(command) and uses it to configure the LCD's operating mode. RS is set to 0 to enable command mode
		and the command is written to the data pins.
		Refer to the LCD Control section in bsp.h to see possible command values
	*/
	LCD_4BitTransfer(command & 0xF0, 0);															//send upper 4 bits of command
	LCD_4BitTransfer(command << 4, 0);																//send lower 4 bits of command
	/*Commands like 0x1(clear screen) require at least 1.25ms while others require only 37µs*/
	if(command < 0x4) {
		delay_us(2000);
	}
	else {
		delay_us(37);
	}
}


void LCD_4BitData(unsigned char data) {
	/*
		This function takes an 8-bit variable(data) and prints it on the LCD.
		RS is set to 1 to enable data mode and the data is written to the data pins.
	*/
	LCD_4BitTransfer(data & 0xF0, 1);																	//send upper 4 bits of data
	LCD_4BitTransfer(data << 4, 1);																		//send upper 4 bits of data
	delay_us(37);
}

void displayString(char *string) {
	/*
		This function takes a string of character and prints it on the LCD.
		Prints the string by iteratively passing individual characters to the LCD_4BitData() function
	*/
		int i = 0;
		while(i < strlen(string)) {
//			if(i > 15) {
//				//delay_us(500000);
//				LCD_4BitCommand(SCROLL);
//				delay_us(500000);
//			}
			LCD_4BitData(string[i]);
			delay_us(1000);
			i++;
		}
}
