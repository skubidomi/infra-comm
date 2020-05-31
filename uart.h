/*
 * uart.h
 *
 * Created: 2020. 05. 26. 17:56:06
 *  Author: Skublics Domonkos
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#ifndef UART_H_
#define UART_H_

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define BAUD 9600
#define MYUBRR ((F_CPU/16/BAUD) - 1) // value for USART Baud Rate Register
#define TX_BUFFER_SIZE 128
#define RX_BUFFER_SIZE 128

void UART_Init(void);
void appendSerial(char c);
void serialWriteNum(uint8_t num);
void serialWrite(char c[]);
char getChar(void);

#endif /* UART_H_ */