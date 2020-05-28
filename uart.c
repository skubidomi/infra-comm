/*
 * uart.c
 *
 * Created: 2020. 05. 26. 18:01:30
 *  Author: Skublics Domonkos
 */ 

#include "uart.h"

char txBuffer[TX_BUFFER_SIZE];
uint8_t txReadPos = 0;
uint8_t txWritePos = 0;

char rxBuffer[RX_BUFFER_SIZE];
uint8_t rxReadPos = 0;
uint8_t rxWritePos = 0;

void UART_Init(void)
{
	UBRR0H = (MYUBRR >> 8); // set baud rate
	UBRR0L = MYUBRR;
	UCSR0B = (1 << TXEN0)  | (1 << TXCIE0) | (1 << RXEN0) | (1 << RXCIE0); // USART Control and Status Register, Tx Complete Interrupt Enable and same with Rx
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void appendSerial(char c)
{
	txBuffer[txWritePos] = c;
	txWritePos++;
	if(txWritePos >= TX_BUFFER_SIZE)
	{
		txWritePos = 0;
	}
}

void serialWrite(char c[])
{
	for(uint8_t i = 0; i < strlen(c); i++)
	{
		appendSerial(c[i]);
	}
	if(UCSR0A & (1 << UDRE0)) // it fires up the interrupt below
	{
		UDR0 = 0;
	}
}

char getChar(void)
{
	char ret = '\0';
	
	if(rxReadPos != rxWritePos)
	{
		ret = rxBuffer[rxReadPos];
		rxReadPos++;
		if (rxReadPos >= RX_BUFFER_SIZE)
		{
			rxReadPos = 0;
		}
	}
	return ret;
}

ISR(USART_TX_vect)
{
	if(txReadPos != txWritePos)
	{
		UDR0 = txBuffer[txReadPos];
		txReadPos++;
		if(txReadPos >= TX_BUFFER_SIZE)
		{
			txReadPos = 0;
		}
	}
}

ISR(USART_RX_vect)
{
	rxBuffer[rxWritePos] = UDR0;
	rxWritePos++;
	if (rxWritePos >= RX_BUFFER_SIZE)
	{
		rxWritePos = 0;
	}
	
}