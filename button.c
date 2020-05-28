/*
 * button.c
 *
 * Created: 2020. 05. 26. 20:02:09
 *  Author: Skublics Domonkos
 */ 

// GOOD TO KNOW: YOU HAVE TO SET THE DDR REGISTER e.g. DDRD &= ~(1 << DDD7);

#include "button.h"

uint16_t ConfidenceLevel;
uint16_t PressedConfidenceLevel;
uint16_t ReleasedConfidenceLevel;
uint8_t Pressed;

void ButtonInit(uint16_t confidenceLevel) // e.g. 500
{
	ConfidenceLevel = confidenceLevel;
	PressedConfidenceLevel = 0;
	ReleasedConfidenceLevel = 0;
	Pressed = 0;
}

uint8_t ButtonPressed(uint8_t PIN_register, uint8_t PIN_bit) // e.g. PIND, PIND7
{
	if (bit_is_set(PIN_register, PIN_bit))
	{
		PressedConfidenceLevel++;
		ReleasedConfidenceLevel = 0;
		if (PressedConfidenceLevel > ConfidenceLevel)
		{
			if (Pressed == 0) // it is a rising edge
			{
				Pressed = 1;
				return 1;
			}
			PressedConfidenceLevel = 0;
		}
	}
	else
	{
		ReleasedConfidenceLevel++;
		PressedConfidenceLevel = 0;
		if (ReleasedConfidenceLevel > ConfidenceLevel)
		{
			Pressed = 0;
			ReleasedConfidenceLevel = 0;
		}
	}
	return 0;
	
}
