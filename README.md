# Tiva_HC05_TFT_integration
Mini project interfacing HC-05 module with the Tiva TM4C123 Launchpad The goal of this project is to interface an HC-O5 Bluetooth module via UART to the Tiva C Launchpad in order to remotely monitor sensor readings and remotely send commands to activate connected output peripherals. Two UART modules are used: UART0 -> communication between PC and TM4C123; UART2 -> communication between HC-O5 and TM4C123 One ADC module, ADC0, is used to read the value from the sensor connected to PE3 (AIN channel 0). The ADC is set to sample every 5 seconds via a timer trigger supplied by TIMER0. The result of the ADC convervsion is sent to the two UART data registers for transmission. This is handled by the ADC Interrupt Handler The SysTick Timer is also used to toggle the on-board blue LED every half-second via its interrupt handler.

//TO DO

Interface more sensors using SPI and/or I²C protocols
Introduce RTOS functionalities and OOP techniques
