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

void PWM_Init(void); // with timer0
void Timer_Init(void); // with timer1
void IR_SendCode(uint32_t code);

#endif /* IR_H_ */