/*
 * ir.h
 *
 * Created: 2020. 05. 26. 17:56:19
 *  Author: Skublics Domonkos
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h" // just for logging or debugging

#ifndef IR_H_
#define IR_H_

// LED toggling macros
#define LED_ON		PORTD |= (1 << PORTD6)
#define LED_OFF		PORTD &= ~(1 << PORTD6)
#define LED_TOGGLE	PIND  |= (1 << PIND6)
#define INTERNAL_LED_TOGGLE PINB |= (1 << PINB5)

// time values in burst time:
#define NEC_LEADING_PULSE 16
#define NEC_LEADING_SPACE 8
#define NEC_PULSE 1
#define NEC_ZERO_SPACE 1
#define NEC_ONE_SPACE 3
#define NEC_BITS 32

// IR receiver defines:
#define IR_SUCCESS 0
#define IR_ERROR 1

#define LOW 0
#define HIGH 1

#define IR_EVENT_IDLE 0
#define IR_EVENT_INIT 1
#define IR_EVENT_PROC 2
#define IR_EVENT_FINI 3

#define IR_PROTO_EVENT_INIT 0
#define IR_PROTO_EVENT_DATA 1
#define IR_PROTO_EVENT_HOOK 2
#define IR_PROTO_EVENT_FINI 3


void PWM_Init(void); // with timer0
void Timer1_Init(void); // with timer1
void IR_SendCode(uint32_t code);

void Timer2_Init(void); // for reading the receiver
void Receiver_Init(void);

uint8_t IR_read(uint8_t *adr, uint8_t *cmd);

#endif /* IR_H_ */