// ADC.h
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0

// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly
#ifndef ADC_H
#define ADC_H
#include <stdint.h>

// ADC initialization function 
// Input: none
// Output: none
// measures from PD2, analog channel 5
void ADC_Init(void);

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
// measures from PD2, analog channel 5
uint32_t ADC_In(void);

/**
*Convert changes voltage in to a uint32_t
*param - input, data to convert
*returns a value roughly between 400-2300. Rounded to 0-2.
*/
uint32_t Convert(uint32_t input);

/**
*Volume Adjust is called by Timer2A_Handler
*reads value of current volume slide/ slide potentiometer and converts into a
*3 decimal point number. Is rounded and adjusts a modifier for playing 
*sound array to enhance/reduce volume
*/
void VolumeAdjust(void);	

/**
*getVolumeModifier is a get method for the play BGM/SFX methods in Sound.c
*returns the static modifier value changed by Volume Adjust interrupt.
*/
uint8_t getVolumeModifier(void);
#endif
