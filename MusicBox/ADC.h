/**
 * ADC.H
 * C, ARM assembly
 * @author: Matthew Yu
 *
 * Provide functions that initialize ADC0
 * Last Modified: 11/28/2018
 * 11/28/18 - standardized comment style, preparing for c++ rewrite
 **/
#ifndef ADC_H
#define ADC_H
#include <stdint.h>

/**
 * ADC_Init
 * measures from PD2, analog channel 5
 **/
void ADC_Init(void);

/**
 * ADC_In - Busy-wait Analog to digital conversion
 * @return - 12-bit result of ADC conversion
 * measures from PD2, analog channel 5
 **/
uint32_t ADC_In(void);

/**
 * Convert - Changes analog voltage in to a uint32_t
 * @param - input, data to convert
 * @return - a value modified from 0-4.
 **/
uint32_t Convert(uint32_t input);

/**
 * VolumeAdjust - converts pot input into a volume modifier
 * volume slide/ slide potentiometer voltage -> 3 d.p. number -> 0-4 modifier
 * called by Timer2A_Handler
 **/
void VolumeAdjust(void);

/**
 * getVolumeModifier - for the play BGM/SFX methods in Sound.c
 * @return - modifier value changed by Volume Adjust interrupt.
 **/
uint8_t getVolumeModifier(void);
#endif
