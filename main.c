/*
 * InfraComm.c
 *
 * Created: 2020. 05. 26. 17:00:01
 * Author : Skublics Domonkos
 */ 

#include <avr/io.h>
#include "uart.h"
#include "ir.h"

int main(void)
{
	DDRD |= (1 << DDD6); // data direction register for IR LED
	DDRB |= (1 << DDB5); // internalLED
	
	DDRD &= ~(1 << DDD7); // data direction register for the receiver, input is like a button with pull up resistor
	
	PWM_Init();
	Timer_Init();
	UART_Init();
	sei();
	
	// ToDo1: organize the ddr macros into the init functions, ir led pin, internal led pin, and button pin
	// ToDo2: organize the init functions into a separate Init function here
	serialWrite("hello");
	IR_SendCode(0x20df10ef);
    while (1) 
    {
		if(!(PIND & (1 << PIND7)))
		{
			INTERNAL_LED_TOGGLE;
			serialWrite("now");
		}
    }
}

