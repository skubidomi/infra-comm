/*
 * ir.c
 *
 * Created: 2020. 05. 26. 18:01:48
 *  Author: Skublics Domonkos
 */ 

#include "ir.h"

volatile uint8_t burst_counter; // for ir transmitting, it counts the proper burst numbers

volatile uint32_t IR_rawdata = 0; // after receiving it "catches" the messages
volatile uint16_t IR_counter = 0; // increased in ISR
volatile uint16_t IR_timeout = 0; // increased in ISR

uint8_t IR_event = 0;
uint8_t IR_proto_event = 0;
uint8_t IR_index = 0;
uint32_t IR_data = 0;

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

void Timer1_Init(void)
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

	// send leading pulse and space
	IR_CarrierON(NEC_LEADING_PULSE);

	IR_CarrierOFF(NEC_LEADING_SPACE);

	// send bits
	for(i=0; i<NEC_BITS; i++)
	{
		IR_CarrierON(NEC_PULSE);
		if(code & 0x80000000) // get the current bit by masking all but MSB
		{
			IR_CarrierOFF(NEC_ONE_SPACE);
		}
		else
		{
			IR_CarrierOFF(NEC_ZERO_SPACE);
		}
		code <<= 1;
	}
	
	// send single STOP bit
	IR_CarrierON(NEC_PULSE);
}

// receiver handler functions:
void Receiver_Init(void)
{ 
	DDRD &= ~(1 << DDD2); // PORTD2 input for the ir receiver output pin
	EIMSK |= (1 << INT0);// enable INT0 interrupt handler -> PORTD2 INPUT
	EICRA |= (1 << ISC00);// trigger INT0 interrupt on raising and falling edge
}

void Timer2_Init(void)
{
	TCCR2A |= (1 << WGM21); // set timer2 to ctc
	TCCR2B |= (1 << CS20); // set no prescaler
	OCR2A = 209; // set timer frequency to get 38 kHz
	TIMSK2 |= (1 << OCIE2A);// enable timer COMPA interrupt
}

uint8_t IR_read(uint8_t *adr, uint8_t *cmd)
{
	if(!IR_rawdata) // if there is no read data
	{
		return IR_ERROR;
	}
	/*uint8_t tmp_adr = IR_rawdata;// last byte of the 32 bit message (the inverted -> correct data)
	uint8_t tmp_cmd = IR_rawdata >> 16;// second byte of the 32 bit message (also the inverted part)
	uint8_t i = 0;
	for (i = 0; i < 8; i++)
	{
		if ()
		{
		}
	}
	// here, original MSB is on the last bit of the rawdata*/
	*cmd = IR_rawdata >> 8;// third byte of the 32 bit message (the inverted -> correct data)
	*adr = IR_rawdata >> 24;// first byte of the 32 bit message (also the inverted part)
	IR_rawdata = 0;
	
	return IR_SUCCESS;
}

static uint8_t IR_NEC_process(uint16_t counter, uint8_t value)// static function - visible just from this compilation unit
{
	uint8_t retval = IR_ERROR;

	switch (IR_proto_event)
	{
	case IR_PROTO_EVENT_INIT:
		if (value == HIGH)
		{
			if (counter > 330 && counter < 360)
			{
				IR_proto_event = IR_PROTO_EVENT_DATA;
				IR_data = 0;
				IR_index = 0;
				retval = IR_SUCCESS;
			}
			else if (counter > 155 && counter < 185) // 2.25ms space for NEC Code repeat
			{
				IR_proto_event = IR_PROTO_EVENT_FINI;
				retval = IR_SUCCESS;
			}
		}
		break;
	case IR_PROTO_EVENT_DATA:
		if (IR_index < 32)
		{
			if(value == HIGH)
			{
				IR_data |= ((uint32_t)((counter < 90) ? 0 : 1) << (31 - IR_index));
				IR_index++;
				if (IR_index == 32) 
				{
					IR_proto_event = IR_PROTO_EVENT_HOOK;
				}
			}
			retval = IR_SUCCESS;
		}
		break;
	case IR_PROTO_EVENT_HOOK:
		// expecting a final 562.5µs pulse burst to signify the end of message transmission
		if (value == LOW)
		{
			IR_proto_event = IR_PROTO_EVENT_FINI;
			retval = IR_SUCCESS; // 0
			serialWrite("fini kell jojjon");
			// ideiglenes eleje
			IR_rawdata = IR_data;
			// ideiglenes vege
		}
		break;
	case IR_PROTO_EVENT_FINI:
		IR_rawdata = IR_data; // copy data to volatile variable, raw data is ready
		break;
	default:
		break;
	}
	
	return retval;
}

static void IR_process(void) // external INT0 interrupt's ISR calls this function
{
	uint8_t value;
	uint16_t counter;
	
	counter = IR_counter; // IR_counter is increased by the 38kHz timer2 interrupt, local preserves the value
	IR_counter = 0;
	
	value = (PIND & (1 << PIND2)) > 0 ? LOW : HIGH; // read the value of the ir pin PORTD2, logical inverse value!
	
	switch (IR_event)
	{
	case IR_EVENT_IDLE:
		// waiting for an initial signal
		if (value == HIGH)
		{
			IR_event = IR_EVENT_INIT;
		}
		break;
	case IR_EVENT_INIT:
		if(value == LOW) // recognize the leading pulse:
		{
			if(counter > 655 && counter < 815) // NEC proto detected // maybe it is 715 based on counter = 0.009/(1.0/38095) * 2 (+/- 30)
			{
				IR_event = IR_EVENT_PROC;
				IR_proto_event = IR_PROTO_EVENT_INIT;
				IR_timeout = 7400;
			}
			else
			{
				IR_event = IR_EVENT_FINI; // goto IDLE state
			}
		}
		else
		{
			IR_event = IR_EVENT_FINI; // goto IDLE state
		}
		break;
	case IR_EVENT_PROC:
		if(IR_NEC_process(counter, value))// decode NEC protocol data
		{
			IR_event = IR_EVENT_FINI;
		}
		break;
	case IR_EVENT_FINI: // clear timeout and set IDLE STATE
		IR_event = IR_EVENT_IDLE;
		IR_timeout = 0;
		break;
	default:
		break;
	}
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

ISR(INT0_vect) // interrupt handler <- called when receiver changes the voltage level
{
	IR_process();
}

ISR(TIMER2_COMPA_vect)
{
	if(IR_counter++ > 10000)
	{
		IR_event = IR_EVENT_IDLE;
	}
	if (IR_timeout && (--IR_timeout == 0))
	{
		IR_event = IR_EVENT_IDLE;
	}
}