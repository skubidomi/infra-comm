/*
 * button.h
 *
 * Created: 2020. 05. 26. 20:01:58
 *  Author: Skublics Domonkos
 */ 

// LIBRARY FOR A BUTTON WITH A PULL DOWN RESISTOR. FOR A RISING EDGE
// IT RETURNS WITH ONE ONLY ONCE

#ifndef BUTTON_H_
#define BUTTON_H_

#include <avr/sfr_defs.h>
#include <avr/io.h>

void ButtonInit(uint16_t confidenceLevel);
uint8_t ButtonPressed(uint8_t PIN_register, uint8_t PIN_bit); // e.g. PIND, PIND7

#endif /* BUTTON_H_ */