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
	
	// variables for the read data:
	uint8_t addr, cmd;
	
	PWM_Init();
	Timer1_Init();
	UART_Init();
	

	Receiver_Init();
	Timer2_Init();
	
	sei();
	
	// ToDo1: organize the ddr macros into the init functions, ir led pin, internal led pin, and button pin
	// ToDo2: organize the init functions into a separate Init function here
	// ToDo3: organize ir read and write function name convention 

	//IR_SendCode(0x20df10ef);
	serialWrite("Init is done\r\n");
	serialWriteNum(255);
    while (1) 
    {
		if (IR_read(&addr, &cmd) == IR_SUCCESS)
		{
			/*serialWriteNum(addr);
			serialWriteNum(cmd);
			serialWrite("\r\n");*/
			if(addr == 0x20 && cmd == 0x10){
				/*serialWriteNum(addr);
				serialWriteNum(cmd);
				serialWrite("\r\n");*/
				INTERNAL_LED_TOGGLE;
			}
			//serialWrite("It works\r\n");
		}
    }
}

