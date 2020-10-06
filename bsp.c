/*-----------------------------------------Board Support Packagefor the EK-TM4C123GXL board------------------------------------------------
Version: 1.1
Author(s): Damola Awesu; Vinay Garade
File Description: Header File containing function definitions for peripheral configuration and interrupt handlers; Used in files: main.c

-----------------------------------------------------------------------------------------------------------------------------------------*/
#include "TM4C123.h"                    // Device header
#include "TM4C123GH6PM.h"
#include "bsp.h"

uint8_t value;
uint32_t distance;
const double clock = 62.5e-9;
const uint32_t factor = 5882;
char x;
//__attribute__((naked)) void assert_failed (char const *file, int line) {
    /* TBD: damage control */
//    NVIC_SystemReset(); /* reset the system */
//}

void setupPeripherals(void) {
	/*Set up all necessary peripherals: UARTs, Timers, ADC and GPIOs*/
	
	/*Enable clock to GPIO, ADC, TIMER and UART modules*/
	SYSCTL->RCGCUART |= UART_CFG;
	SYSCTL->RCGCTIMER |= TIMER_CFG;
	SYSCTL->RCGCADC |= ADC_CFG;
	SYSCTL->RCGCGPIO |= GPIO_CFG;
	
	/*Configure pins PA0 and PA1 for alternate digital functions (UART RX and TX pins)*/
	GPIOA->AFSEL |= UART0_TX_RX;
	GPIOA->AMSEL &= ~UART0_TX_RX;
	GPIOA->PCTL = UART0_PTCL;
	GPIOA->DEN |= UART0_TX_RX;
	
	/*Configure Trigger Pin of HC-SR04 as PB3*/
	GPIOB->DIR |= TRIGGER_PIN;
	GPIOB->DEN |= TRIGGER_PIN;
	
	/*Configure pins for UART2 RX and TX (Bluetooth connection)*/
	GPIOD->LOCK = LOCK_KEY;																					//unlock PD7 for GPIO function
	GPIOD->CR = CR_UNLOCK;																					//commit PD7 as GPIO pin
	GPIOD->AFSEL |= UART2_TX_RX;																		//enable alternate function for PD6 & PD7
	GPIOD->AMSEL &= ~UART2_TX_RX;
	GPIOD->PCTL = UART2_PTCL;																				//select UART function for PD6 & PD7
	GPIOD->DEN |= UART2_TX_RX;
	
	/*Set up PE3 as analog input */
	GPIOE->AFSEL &= ~ADC_IN;   																			//disable alternate function for PE3
	GPIOE->DEN &= ~ADC_IN;																					//disable digital function
	GPIOE->AMSEL |= ADC_IN;
	
	/*Set up the RGB LEDs*/
	GPIOF->DIR |= LED_RED|LED_BLUE|LED_GREEN;
	GPIOF->DEN |= LED_RED|LED_BLUE|LED_GREEN;
	GPIOF->DATA &= ~LED_RED|LED_BLUE|LED_GREEN;
	
	/*Set up SysTick Timer*/
	//Configured to overflow every half-second
	SysTick->LOAD = SYSTICK_LOAD;
	SysTick->VAL  = 0U;
	SysTick->CTRL = SYSTICK_EN;
	
	/*Timer0A Setup */
	//Timer0A is used as trigger for ADC0 conversion
	TIMER0->CTL &= ~(1U << 0);
	TIMER0->CFG = MODE_32BIT;
	TIMER0->TAMR |= TIMER_MODE;
	TIMER0->TAILR = TIMER_LOAD;
	TIMER0->IMR = (1 << 0);
	TIMER0->ICR |= (1 << 0);
	TIMER0->CTL |= TIMER_EN;	
	
	/* ADC0 Config */
	ADC0->ACTSS &= ~SS3;  																				//disable Sample Sequencer 3 for configuration
	ADC0->EMUX &= ~(0xF << 12);
	ADC0->EMUX |= ADC_TRIGGER;   																	//set SS3 to sample on timer interrupt event
	ADC0->SSMUX3 = 0;																							//select AIN channel 0 (PE3)
	ADC0->SSCTL3 |= ADC_CTL;																			//configure SS3 for 1 sample and end sampling after sample
	ADC0->IM |= SS3;																							//configure interrupt
	ADC0->ACTSS |= SS3;
	
	/*Configure UART0 with following properties: Baud rate of 9600; 8-bit data; No parity, 1 stop bit; system clock as source*/
	UART0->CTL &= ~(1 << 0);
	UART0->IBRD = BAUDINT_9600;																		//Baud rate of 9600
	UART0->FBRD = BAUDFR_9600;																		//Baud rate of 9600
	UART0->LCRH = UART_MODE;																			//8-bit data; No parity, 1 stop bit
	UART0->CC = SYS_CLK_SRC;																							//select system clock as source
	UART0->CTL = UART_EN;
	
	/*Configure UART2 with properties: Baud Rate: 9600; Data: 8-bit; No Parity, 1 stop bit; System clock as source*/
	UART2->CTL &= ~(1 << 0);
	UART2->IBRD = BAUDINT_9600;																		//Baud rate of 9600
	UART2->FBRD = BAUDFR_9600;																		//Baud rate of 9600
	UART2->LCRH = UART_MODE;																			//8-bit data; No parity, 1 stop bit
	UART2->IM |= RX_INT;																				//enable RX interrupt
	UART2->CC = SYS_CLK_SRC;																							//select system clock as source
	UART2->CTL = UART_EN;
	
	/*Set up ADC0 Seq3 Interrupts in the NVIC*/
	NVIC->IP[17] |= (4 << 5);
	NVIC->ISER[0] |= (1 << 17);
	
	NVIC->IP[19] |= (3 << 5);
	NVIC->ISER[0] |= (1 << 19);
	
	/*Set up UART2 Interrupts in the NVIC*/
	NVIC->IP[33] |= (2 << 5);
	NVIC->ISER[1] |= (1 << 1);
	
	/*Enable Global Interrupts*/
	__enable_irq();
	
}

void SysTick_Handler(void) {
	//disable interrupts to avoid race conditions and re-enable after instructions have been carried out
		__disable_irq();
    GPIOF->DATA ^= LED_BLUE;
		__enable_irq();
}

void ADC0SS3_Handler(void) {
	printString("Resistance Value: ");
	printBTString("Resistance Value: ");
	value = (ADC0->SSFIFO3)*100/4096;														//map value of ADC result on a scale of 0 - 100
	LCD_4BitCommand(LINE_1);
	delay_us(1000);
	displayString("ADC Val: ");
	//printChar(value);
	//printString("\n\r");
	printf("Resistance value: %d\r\n", value);
	printBTInt(value);
	printBTString("\n\r");
	ADC0->ISC |= (1 << 3);																				//acknowledge ADC interrupt
}

void UART2_Handler(void) {
	//check if RX holding register has data or if there is RX timeout
	if((UART2->RIS & (1 << 4)) || (UART2->RIS & (1 << 6))) {
		x = UART2->DR;																							//copy received data
		switch(x) {
			case 'r':
				GPIOF->DATA = LED_RED;
				break;
			case 'g':
				GPIOF->DATA = LED_GREEN;
				break;
			default:
				GPIOF->DATA &= ~(LED_RED|LED_GREEN);
				break;
		}
		UART2->ICR |= (1 << 4);																			//acknowledge UART interrupt
	}
}

int readChar(void) {
	//read and return 8-bit data from the UART(PC) data register
	char dat;
	while((UART0->FR & (1 << 4)) != 0);
	dat = UART0->DR;
	return dat;
}

int printChar(char c) {
	//transmit data to the PC terminal
	while((UART0->FR & (1 << 5)) != 0);
	UART0->DR = c;
	return UART0->DR;
}

void printString(char *string) {
	//print string of data on PC terminal character by character
	while(*string) {
		printChar(*(string++));
	}
}

char readBTChar(void) {
	//read and return 8-bit data from the UART2(Bluetooth) data register
	char dat;
	while((UART2->FR & (1 << 4)) != 0);
	dat = UART2->DR;
	return dat;
}

void printBTChar(char c) {
	//transmit data to the Bluetooth terminal
	while((UART2->FR & (1 << 5)) != 0);
	UART2->DR = c;
}

void printBTInt(uint8_t c) {
	//convert integer data to ASCII value and transmit data to the Bluetooth terminal
	char temp[3];
	int i = 2;
	/**/
	//delay_us(1000);
	/**/
	while(i >= 0) {
		if((c%10)!= 0){
			temp[i] = ((c % 10)+'0');
		}
		else {
			temp[i] = '0';
		}
		c = c/10;
		i--;
	}
	displayString(temp);
	printBTString(temp);
}

void printBTString(char *string) {
	//print string of data on Bluetooth terminal character by character
	while(*string) {
		printBTChar(*(string++));
	}
}

uint32_t measureDistance(void) {
	//calculate the distance from an object
	uint32_t distance, h_edge, l_edge;														//h_edge : stores the timer value when the ECHO pin goes high
																																//l_edge : stores the timer value when the ECHO pin goes low
	echoTimer();
	//this sequence generates the pulse to be received by the ECHO PIN using the TRIGGER pin
	GPIOB->DATA &= ~TRIGGER_PIN;
	delay_us(10);
	GPIOB->DATA |= TRIGGER_PIN;
	delay_us(10);
	GPIOB->DATA &= ~TRIGGER_PIN;
	
	//Capture the rising edge of the ECHO pin and save timer value in h_edge
	TIMER1->ICR = (1 << 2);
	while((TIMER1->RIS & (1 << 2))== 0){};
	h_edge = TIMER1->TAR;
	
	//Capture the falling edge of the ECHO pin and save timer value in l_edge
	TIMER1->ICR = (1 << 2);
	while((TIMER1->RIS & (1 << 2))== 0){};
	l_edge = TIMER1->TAR;
	
	//Compute and return the distance in cm
	distance = l_edge - h_edge;
	distance = clock * (double)factor * (double)distance;
	
	return distance;
}

void delay_us(uint32_t delay) {
	//Create a µsecond timer
	TIMER2->CTL &= ~(1 << 0);
	TIMER2->CFG |= MODE_32BIT;
	TIMER2->TAMR |= 0x02;
	TIMER2->TAILR = (SYS_CLOCK_HZ/1000000 - 1);
	TIMER2->IMR &= ~(1 <<0);
	TIMER2->CTL |= (1 << 0);
	
	for(int i = 0; i < delay; i++) {
		while((TIMER2->RIS & (1 << 0)) == 0);
		TIMER2->ICR |= (1 << 0);
	}
	
}

void echoTimer(void) {
	//Configure the timer to capture rising and falling edges of the ECHO pin
	SYSCTL->RCGCTIMER |= (1 << 1);
	//configure PB4 as ECHO pin
	GPIOB->DIR &= ~ECHO_PIN;
	GPIOB->AFSEL |= ECHO_PIN;																			//enable alternate functions
	GPIOB->DEN |= ECHO_PIN;
	GPIOB->PCTL &= ~0x000F0000;
	GPIOB->PCTL |= T1CCP0_MODE;																		//select T1CCP0 alternate function
	//configure Timer1 for edge timing
	TIMER1->CTL &= ~(1U << 0);
	TIMER1->CFG = MODE_16BIT;
	TIMER1->TAMR |= CAPTURE_MODE;
	TIMER1->CTL |= RF_EDGE;
	TIMER1->CTL |= (1 << 0);
	TIMER1->ICR |= CAPTURE_INT;
}

void ultrasonic(void){
	//calculate distance and transmit over bluetooth
	distance = measureDistance();
	delay_us(500000);
	LCD_4BitCommand(LINE_2);
	delay_us(1000);
	displayString("Distance: ");
	printBTString("Distance: ");
	printBTInt(distance);
	printBTString("\n\r");
	//control red LED based on distance value
	if(distance < MAX_DISTANCE){
		GPIOF->DATA |= LED_RED;
	}
	else {
		GPIOF->DATA &= ~LED_RED;
	}
}

void TIMER0A_Handler(void) {
	//Run the ultrasonic function when Timer0A interrupt occurs
	ultrasonic();
	TIMER0->ICR |= (1 << 0);																			//acknowledge Timer0A interrupt
}

struct __FILE { int handle;};
FILE __stdin 	= {0};
FILE __stdout = {1};
FILE __stderr = {2};

int fgetc(FILE *f) {
	int c;
	c = readChar();
	
	if(c == '\r') {
		printChar(c);
		c = '\n';
	}
	printChar(c);
	
	return c;
}

int fputc(int c, FILE *f) {
	return printChar(c);
}
