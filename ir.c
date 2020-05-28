/*
 * ir.c
 *
 * Created: 2020. 05. 26. 18:01:48
 *  Author: Skublics Domonkos
 */ 

#include "ir.h"

volatile uint8_t burst_counter;

void PWM_Init(void)
{
	// timer0
	TCCR0A |= (1 << WGM01); // CTC mode
	TCCR0B |= (1 << CS00)/* | (1 << CS02)*/; // set no prescaler, IDEIGLENESEN PRESCALING
	
	OCR0A = 209; // We set it to 38kHz: OCR0A = (f_clk / (f_ocr0a * 2 * 1)) - 1
	//OCR0A = 77; //100Hz, to get ~1 sec, we need a counter to 100
}

void PWM_StartToggling(void)
{
	TIMSK0 |= (1 << OCIE0A);// Output Compare Interrupt Enable A
}

void PWM_StopToggling(void)
{
	TIMSK0 &= ~(1 << OCIE0A); // Output Compare Interrupt disable A
	LED_OFF;
}

void Timer_Init(void)
{
	// timer1
	TCCR1B |= (1 << WGM12); // CTC mode
	OCR1A = 8999; // set compare value for 562.5 us
}

void IR_CarrierON(uint8_t length) // number of burst_lengths
{
	burst_counter = 0;
	// start timer:
	TCCR1B |= (1 << CS10)/* | (1 << CS12)*/; // no prescaling IDEIGLENESEN PRESCALING
	TCNT1 = 0; // timer set to zero
	TIMSK1 |= (1 << OCIE1A); // Output Compare Interrupt Enable A
	
	PWM_StartToggling();
	while(burst_counter < length)
	{
	}
	PWM_StopToggling();

	// stop timer:
	TCCR1B &= ~(1 << CS10); // no clock source
	TIMSK1 &= ~(1 << OCIE1A); // interrupt disable
}

void IR_CarrierOFF(uint8_t length) // number of burst_lengths
{
	burst_counter = 0;
	// start timer:
	TCCR1B |= (1 << CS10)/* | (1 << CS12)*/; // no prescaling IDEIGLESEN PRESCALING
	TCNT1 = 0; // timer set to zero
	TIMSK1 |= (1 << OCIE1A); // Output Compare Interrupt Enable A
	
	while(burst_counter < length)
	{
	}

	// stop timer:
	TCCR1B &= ~(1 << CS10); // no clock source
	TIMSK1 &= ~(1 << OCIE1A); // interrupt disable
}

void IR_SendCode(uint32_t code)
{
	uint8_t i;
	char c[2] ;
	// send leading pulse and space
	IR_CarrierON(NEC_LEADING_PULSE);

	c[0] = burst_counter;
	c[1] = '\0';
	//serialWrite("Leading pulse: ");
	serialWrite(c);

	IR_CarrierOFF(NEC_LEADING_SPACE);

	c[0] = burst_counter;
	c[1] = '\0';
	serialWrite(c);

	// send bits
	for(i=0; i<NEC_BITS; i++)
	{
		IR_CarrierON(NEC_PULSE);

		c[0] = burst_counter;
		c[1] = '\0';
		serialWrite(c);

		if(code & 0x80000000) // get the current bit by masking all but MSB
		{
			IR_CarrierOFF(NEC_ONE_SPACE);

			c[0] = burst_counter;
			c[1] = '\0';
			serialWrite(c);
		}
		else
		{
			IR_CarrierOFF(NEC_ZERO_SPACE);

			c[0] = burst_counter;
			c[1] = '\0';
			serialWrite(c);

		}
		code <<= 1;
	}
	
	// send single STOP bit
	IR_CarrierON(NEC_PULSE);
}

ISR(TIMER0_COMPA_vect)
{
	LED_TOGGLE;
}

ISR(TIMER1_COMPA_vect)
{
	burst_counter++;
	INTERNAL_LED_TOGGLE;
}