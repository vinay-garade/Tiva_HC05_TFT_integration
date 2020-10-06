/*---------------------------------Board Support Package for the EK-TM4C123GXL board-------------------------------------
Version: 1.1
Author(s): Damola Awesu; Vinay Garade
File Description: Header File containing mqcros and function prototypes used in files: bsp.c, lcd.c

-----------------------------------------------------------------------------------------------------------------------*/
#ifndef __BSP_H__
#define __BSP_H__

#include <stdio.h>
/* System Values */
#define SYS_CLOCK_HZ 	16000000U
#define LOCK_KEY			0x4C4F434B
#define CR_UNLOCK			0xFF

/*Ultrasonic Sensor Macros*/
#define TRIGGER_PIN		(1 << 3)
#define ECHO_PIN			(1 << 4)
#define T1CCP0_MODE		0x00070000
#define MAX_DISTANCE	0xF

/*RCGC Macros*/
#define GPIO_CFG			(1 << 0)|(1 << 1)|(1 << 2)|(1 << 3)|(1 << 4)|(1 << 5)
#define TIMER_CFG			(1 << 0)|(1 << 2)
#define ADC_CFG				(1 << 0)
#define UART_CFG			(1 << 0)|(1 << 2)

/*SysTick Macros*/
#define SYSTICK_LOAD	SYS_CLOCK_HZ/2U - 1U
#define SYSTICK_EN		(1U << 2) | (1U << 1) | 1U

/*TIMER Macros*/
#define TIMER_MODE		0x02
#define CAPTURE_MODE	0x17
#define MODE_32BIT		(0x0 << 0)
#define MODE_16BIT		(0x4 << 0)
#define RF_EDGE				(0x3 << 2)
#define CAPTURE_INT		(1 << 2)
#define TIMER_LOAD		SYS_CLOCK_HZ*10U - 1U
#define TIMER_EN			(1U << 5) | (1U << 0)

/*UART Macros*/
#define BAUDINT_9600	104
#define BAUDFR_9600		11
#define UART_MODE			(0x3 << 5)
#define SYS_CLK_SRC		0x05
#define UART0_TX_RX		0x03
#define UART0_PTCL		0x11
#define UART2_TX_RX 	0xC0
#define UART2_PTCL		0x11000000
#define RX_INT				(1 << 4)
#define UART_EN				(1<<9)|(1<<8)|(1<<0)

/*LCD 8-bit Macros*/
#define CMD_CFG				0xE0
#define DATA_CFG			0xFF
#define EN_PIN_HIGH		0x80
#define DATA_SELECT		0x20
#define CMD_SELECT		0x00
#define LCD_RESET			0xE0
#define CLR_SCR				0x01
#define CURS_HOME			0x02
#define CURSOR_SHIFT	0x06
#define SCROLL				0x18
#define SPACE_BAR			0x14
#define LCD_ON				0x0C
#define LINE_1				0x80
#define LINE_2				0xC0
#define MODE_8BIT			0x38
#define MODE_4BIT			0x28

/*ADC Macros*/
#define ADC_IN				(1U << 3)
#define SS3						(1U << 3)
#define ADC_TRIGGER		(0x5 << 12)
#define ADC_CTL				(1U << 1) | (1U << 2)

/* on-board LEDs */
#define LED_RED   		(1U << 1)
#define LED_BLUE  		(1U << 2)
#define LED_GREEN 		(1U << 3)

extern uint32_t distance;
/*-------------------------------Function Declarations--------------------------------*/

/*Common Functions*/
void setupPeripherals(void);
void delay_us(uint32_t delay);

/*PC UART Functions*/
int readChar(void);
int printChar(char c);
void printString( char *string);

/*Bluetooth UART Functions*/
char readBTChar(void);
void printBTChar(char c);
void printBTInt(uint8_t c);
void printBTString(char *string);

/*Distance Measurement Functions*/
uint32_t measureDistance(void);
void echoTimer(void);
void ultrasonic(void);

/*LCD Control Functions*/
void LCD_Init(void);
void displayString(char *string);
void LCD_4BitTransfer(unsigned char data, unsigned char reg_select);
void LCD_4BitCommand(unsigned char command);
void LCD_4BitData(unsigned char data);


#endif // __BSP_H__
